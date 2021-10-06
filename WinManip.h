#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT	0x0500
#include <windows.h>
#include <atlimage.h>
#include ".\DS2Win\arm9\keyboard\source\keyboard.def.h"

void sendKey (WORD key, bool virtualKey, bool down);
void sendVirtualKey (WORD key, bool down);
void setMouse(int x, int y, bool relative = false);
void sendMouseInput(DWORD flag);
void clickMouseDown();
void clickMouseUp();
void clickRightMouseDown();
void clickRightMouseUp();
void clickMiddleMouseDown();
void clickMiddleMouseUp();
bool screenCap(bool full, int x, int y);
void getScreenRes (unsigned short* width, unsigned short* height);
