#pragma once
#include <Winsock2.h>
#include <stdio.h>
#include "netdata.h"
#include "WinManip.h"

#define CONNTIMEOUT	5000

extern void debugPrint (char * txt, ...);

enum DISC_REASONS {
	DISC_QUIT,
	DISC_TIMEOUT,
	DISC_SHUTDOWN
};

struct SCREENSHOTDATA {
	int size;
	unsigned char sendCommand;
	unsigned char sendBlockCommand;
	unsigned char screenBuf[32768];
};

class CServer
{
private:
	int mPort, mPingDelay, mPingTimeout, mWaitConnectTime;
	unsigned int screenSize;
	SOCKET mSock;
	FILE* screen;
	struct sockaddr_in sain, saout;
	WSADATA wsaData;
	char inBuf[4096];
	char outBuf[4096];
	char mPassword[16];
	fd_set socks;
	bool mIsRunning, mConnected;
	struct SCREENSHOTDATA screenFull, screenPart;
	unsigned char lastMouseX, lastMouseY;
public:
	CServer(int port);
	~CServer(void);
	bool open(char* password);
	void close();
	void update();
	void handlePacket(BYTE command, char* data, int datalen);
	bool sendScreenShot(bool full, unsigned short x, unsigned short y, struct SCREENSHOTDATA* buffer);
	void sendScreenBlock(unsigned char block, struct SCREENSHOTDATA* buffer);
	void disconnectClient (int reason);
	bool isRunning() {return mIsRunning;}
	bool isConnected() {return mConnected;}
};
