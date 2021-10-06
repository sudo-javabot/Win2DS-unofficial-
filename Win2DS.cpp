// Win2DS.cpp : By Bill Blaiklock (Sintax)
//
// Source is free to use in your own applications, but please give credit where credit is due.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <atlimage.h>
#include <shellapi.h>

#include "server.h"
#include "Win2DS.h"

#define MAX_LOADSTRING 100
#define WM_ICON 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hWnd;
CServer* server;
bool wndHide = false;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void readRegistry () {
	HKEY softwareKey;
	CRegKey appKey;
	DWORD run;
	char pw[16];
	ULONG pwLen = 16;

	RegOpenKey (HKEY_LOCAL_MACHINE, "Software\\", &softwareKey);
	if (appKey.Open(softwareKey,"Win2DS",KEY_READ | KEY_WRITE) != ERROR_SUCCESS)
		return;
	
	if ((appKey.QueryDWORDValue("StartupRun", run) == ERROR_SUCCESS) && run == 1)
			SendMessage (GetDlgItem(hWnd, IDC_CHECKSTARTUP), BM_SETCHECK, BST_CHECKED,0);

	if (appKey.QueryStringValue("Pass", pw, &pwLen) == ERROR_SUCCESS) {
		pw[15] = '\n';
		SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), WM_SETTEXT, 0,(LPARAM) pw);	
	}
	
	appKey.Close();
}

void writeRegistry () {
	HKEY softwareKey;
	CRegKey appKey;
	char pw[16];

	RegOpenKey (HKEY_LOCAL_MACHINE, "Software\\", &softwareKey);
	if (appKey.Create(softwareKey,"Win2DS",REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ | KEY_WRITE,NULL,NULL) != ERROR_SUCCESS)
		return;
	
	
	SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), WM_GETTEXT, 16,(LPARAM) pw);
	pw[15] = '\n';
	appKey.SetStringValue ("Pass", pw);

	if (SendMessage (GetDlgItem(hWnd, IDC_CHECKSTARTUP), BM_GETCHECK,0,0)==BST_CHECKED)
		appKey.SetDWORDValue ("StartupRun", 1);
	else 
		appKey.SetDWORDValue ("StartupRun", 0);
	

	appKey.Close();
}


void debugPrint (char * txt, ...) {
	char buffer[256];
	int lineLen;
	va_list args;
	HWND editbox = GetDlgItem(hWnd, IDC_OUTPUT);
	
	lineLen = (int) SendMessage (editbox, EM_GETLINECOUNT, 0, 0);
	if (lineLen > 200) {
		SendMessage (editbox, EM_SETSEL, 0, 100);
		SendMessage (editbox, EM_REPLACESEL, false, (LPARAM) "");
	}

	va_start(args,txt);
	vsprintf(buffer,txt,args);
	
	strcat (buffer, "\r\n");
	

	SendMessage (editbox, EM_SETSEL, 99999, 99999);
	SendMessage (editbox, EM_REPLACESEL, 0, (LPARAM)buffer);
}

void showTrayIcon (bool hide) {
	NOTIFYICONDATA trayIcon;
	
	trayIcon.cbSize = sizeof(NOTIFYICONDATA);
	trayIcon.hWnd = hWnd;
	trayIcon.uID = IDI_DSWINMIN;
	trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	trayIcon.uCallbackMessage = WM_ICON;
	trayIcon.hIcon = (HICON) LoadIcon(hInst, (LPCTSTR)IDI_DSWINMIN);
	strcpy (trayIcon.szTip, "Win2DS");
	trayIcon.dwState = NIS_SHAREDICON;
	trayIcon.dwStateMask = NIS_SHAREDICON;
	strcpy (trayIcon.szInfo, "");
	trayIcon.uTimeout = 0;
	trayIcon.uVersion = NOTIFYICON_VERSION;
	strcpy (trayIcon.szInfoTitle, "");
	trayIcon.dwInfoFlags = 0;

	if (hide && !wndHide && Shell_NotifyIcon (NIM_ADD, &trayIcon)) {
		ShowWindow (hWnd, SW_HIDE);
		wndHide = true;
	} else if (!hide && wndHide && Shell_NotifyIcon (NIM_DELETE, &trayIcon)) {
		ShowWindow (hWnd, SW_SHOWDEFAULT);
		BringWindowToTop (hWnd);
		SendMessage (GetDlgItem(hWnd, IDC_OUTPUT), EM_SETSEL, 99999, 99999);
		wndHide = false;
	}
}

void StartServer() {
	char pw[16];
	SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), WM_GETTEXT, 16,(LPARAM) pw);
	if (server->open(pw)) {
		SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), EM_SETREADONLY, true, 0);
		SendMessage (GetDlgItem(hWnd, IDC_STARTBUTTON), WM_SETTEXT, 0,(LPARAM) "Stop Server");	
		Sleep (1000);
		CloseWindow(hWnd);
	}
}

void StopServer() {
	server->close();
	SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), EM_SETREADONLY, false, 0);
	SendMessage (GetDlgItem(hWnd, IDC_STARTBUTTON), WM_SETTEXT, 0,(LPARAM) "Start Server");	
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN2DS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateDialog (hInst, (LPCTSTR)IDD_MAIN, 0, (DLGPROC)WndProc);

	if (!hWnd)
		return FALSE;

	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(hInst, (LPCTSTR)IDI_DSWINMIN));
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(hInst, (LPCTSTR)IDI_DSWIN));
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WIN2DS);
	
	SendMessage (GetDlgItem(hWnd, IDC_PASSWORD), EM_SETLIMITTEXT, 15, 0);
	SetFocus(GetDlgItem(hWnd, IDC_PASSWORD));
	
	readRegistry();

	server = new CServer(8888);
	
	if (SendMessage (GetDlgItem(hWnd, IDC_CHECKSTARTUP), BM_GETCHECK,0,0)==BST_CHECKED)
		StartServer();

	// Main loop
	// The server loop shouldn't be in here, because it stops if you mess with the window, but what the hell.
	do
	{
		server->update();
		
		if (!wndHide && IsIconic (hWnd))
			showTrayIcon(true);
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) || !IsDialogMessage(hWnd, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		SleepEx (1, true);	//This probably causes some problems, and is not the way it should be done, but I don't know how else.
	} while (msg.message!=WM_QUIT); 

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DSWIN);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_WIN2DS;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_DSWINMIN);

	return RegisterClassEx(&wcex);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_STARTBUTTON:
			if (server->isRunning())
				StopServer();
			else
				StartServer();
			break;
		}
		break;
	case WM_SYSCOMMAND:
//		if (wParam==SC_MINIMIZE) {
			
//		}
		break;
	case WM_ICON:
		if (lParam==WM_LBUTTONDBLCLK) {
			showTrayIcon(false);
		}
		break;
	case WM_CLOSE:
		writeRegistry();
	case WM_DESTROY:
		server->close();
		showTrayIcon (false);
		DestroyWindow (hWnd);
		PostQuitMessage(0);
		break;
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
