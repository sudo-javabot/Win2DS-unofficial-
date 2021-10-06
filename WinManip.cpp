#include "WinManip.h" 

void sendVirtualKey (WORD key, bool down) {
	switch (key) {
		case F_1:
			sendKey(VK_F1, true, down); break;
		case F_2:
			sendKey(VK_F2, true, down); break;
		case F_3:
			sendKey(VK_F3, true, down); break;
		case F_4:
			sendKey(VK_F4, true, down); break;
		case F_5:
			sendKey(VK_F5, true, down); break;
		case F_6:
			sendKey(VK_F6, true, down); break;
		case F_7:
			sendKey(VK_F7, true, down); break;
		case F_8:
			sendKey(VK_F8, true, down); break;
		case F_9:
			sendKey(VK_F9, true, down); break;
		case F10:
			sendKey(VK_F10, true, down); break;
		case F11:
			sendKey(VK_F11, true, down); break;
		case F12:
			sendKey(VK_F12, true, down); break;
		case HOM:
			sendKey(VK_HOME, true, down); break;
		case PGU:
			sendKey(VK_PRIOR, true, down); break;
		case PGD:
			sendKey(VK_NEXT, true, down); break;
		case END:
			sendKey(VK_END, true, down); break;
		case TAB:
			sendKey(VK_TAB, true, down); break;
		case ESC:
			sendKey(VK_ESCAPE, true, down); break;
		case BSP:
			sendKey(VK_BACK, true, down); break;
		case RET:
			sendKey(VK_RETURN, true, down); break;
		case CRU:
			sendKey(VK_UP, true, down); break;
		case CRD:
			sendKey(VK_DOWN, true, down); break;
		case CRL:
			sendKey(VK_LEFT, true, down); break;
		case CRR:
			sendKey(VK_RIGHT, true, down); break;
		case INS:
			sendKey(VK_INSERT, true, down); break;
		case DEL:
			sendKey(VK_DELETE, true, down); break;
		case SCN:
		case NDS:
		case ALT:
		case CTL:
		case SHF:
		case CAP:
		case EXT:
			break;
		default:
			sendKey(VkKeyScan(key), true, down);
	}	
}
void sendKey (WORD key, bool virtualKey, bool down) {
	KEYBDINPUT keyStroke;
	INPUT outInput;

	if (virtualKey) {
		keyStroke.wVk = (WORD) key;
		keyStroke.wScan = 0;
		if (down)
			keyStroke.dwFlags = 0;
		else
			keyStroke.dwFlags = KEYEVENTF_KEYUP;
	} else {
		keyStroke.wVk = 0;
		keyStroke.wScan = (WORD) key;
		if (down)
			keyStroke.dwFlags = KEYEVENTF_UNICODE;
		else
			keyStroke.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	}
	
	keyStroke.time = 0;
	keyStroke.dwExtraInfo = 0;

	outInput.type = INPUT_KEYBOARD;
	outInput.ki = keyStroke;

	SendInput (1, &outInput, sizeof (outInput));
}

void setMouse (int x, int y, bool relative) {
	MOUSEINPUT moveMouse;
	INPUT outInput;                    

	moveMouse.dx = x;
	moveMouse.dy = y;
	moveMouse.mouseData = 0;
	if (relative)
		moveMouse.dwFlags = MOUSEEVENTF_MOVE;
	else
		moveMouse.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	moveMouse.time = 0;
	moveMouse.dwExtraInfo = 0;

	outInput.type = INPUT_MOUSE;
	outInput.mi = moveMouse;

	SendInput (1, &outInput, sizeof (outInput));
}

void sendMouseInput (DWORD flag) {
	MOUSEINPUT mouseDown;
	INPUT outInput;

	mouseDown.dx = 0;
	mouseDown.dy = 0;
	mouseDown.mouseData = 0;
	mouseDown.dwFlags = flag;
	mouseDown.time = 0;
	mouseDown.dwExtraInfo = 0;

	outInput.type = INPUT_MOUSE;
	outInput.mi = mouseDown;

	SendInput (1, &outInput, sizeof (outInput));
}

void clickMouseDown() {sendMouseInput(MOUSEEVENTF_LEFTDOWN);}
void clickMouseUp() {sendMouseInput(MOUSEEVENTF_LEFTUP);}
void clickRightMouseDown() {sendMouseInput(MOUSEEVENTF_RIGHTDOWN);}
void clickRightMouseUp() {sendMouseInput(MOUSEEVENTF_RIGHTUP);}
void clickMiddleMouseDown() {sendMouseInput(MOUSEEVENTF_MIDDLEDOWN);}
void clickMiddleMouseUp() {sendMouseInput(MOUSEEVENTF_MIDDLEUP);}


void getScreenRes (unsigned short* width, unsigned short* height) {
	HWND pWnd; 
	RECT rect;
	pWnd = GetDesktopWindow(); 

	if (pWnd == NULL) 
		return;

	if (GetWindowRect(pWnd, &rect)==0)
		return;

	*width = rect.right-rect.left;
	*height = rect.bottom-rect.top;
}

bool screenCap (bool full, int x, int y) {
	CImage image, screenImage;
	HWND pWnd;
	RECT rect;
	HDC winDC;
	POINT cursorPos;
	HCURSOR mouseIcon;

//	if (lpData->bCaptureFullScreen)
	pWnd = GetDesktopWindow(); 
//	else
//	pWnd = CWnd::GetActiveWindow(); 

	if (pWnd == NULL) 
		return false;

    winDC = GetDC(NULL);
	if (winDC == NULL)
		return false;
	if (GetWindowRect(pWnd, &rect)==0)
		return false;
	
//	int nBPP = winDC.GetDeviceCaps(BITSPIXEL) * winDC.GetDeviceCaps(PLANES); 
//	if (nBPP < 24) 
//	nBPP = 24; 


	if (!image.Create(256, 192, 16))
		return false;

	if (!screenImage.Create(rect.right-rect.left, rect.bottom-rect.top, 16))
		return false;
	
	CImageDC imageDC(image);
	CImageDC screenImageDC(screenImage);

	BitBlt(screenImageDC, 0, 0, rect.right-rect.left, rect.bottom-rect.top, winDC, 0, 0, SRCCOPY);

	if (GetCursorPos(&cursorPos)) {
		if (mouseIcon = (HCURSOR) LoadImage(NULL, MAKEINTRESOURCE(32512), IMAGE_CURSOR, 0, 0, LR_SHARED)) {
			DrawIcon (screenImageDC, cursorPos.x-10, cursorPos.y-10, mouseIcon);
		}
	}
	
	if (full)
		StretchBlt(imageDC, 2, 2, 252, 188, screenImageDC, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SRCCOPY);
	else
		BitBlt(imageDC, 0, 0, 256, 192, screenImageDC, x, y, SRCCOPY);

	HRESULT hr = image.Save("ss.jpg"); 
	if (FAILED(hr))
		return false;
//	TRACE("Couldn't Save File: %s, %x\n", (LPCTSTR)strTempName, hr); 
	return true;
}