#include "server.h"


CServer::CServer(int port) {
	saout.sin_family=sain.sin_family=AF_INET;
	saout.sin_addr.s_addr=sain.sin_addr.s_addr=0;
	saout.sin_port=sain.sin_port=htons(port);

	mPort = port;
	mIsRunning = false;
	mConnected = false;
	mPingDelay = 0;
	mPingTimeout = 0;
	mSock = 0;
	screenSize = 0;
	lastMouseX = lastMouseY = 0;

	screenFull.sendCommand = SCREENSHOT;
	screenFull.sendBlockCommand = SCREENSHOTDATA;
	screenPart.sendCommand = SCREENSHOTPART;
	screenPart.sendBlockCommand = SCREENSHOTPARTDATA;
}

void CServer::update() {
	int datalen;
	struct sockaddr_in sender;
	int sain_len=sizeof(sender);
	
	if (!mIsRunning)
		return;
	
	datalen=recvfrom(mSock,inBuf,1023,0, (struct sockaddr*)&sender, &sain_len);
	if(datalen > 0) {
		if (inBuf[0]==CONNECT) {
			outBuf[0] = CONNECT;
			if (strcmp(&inBuf[1],mPassword)==0) {
				debugPrint("Connection accepted from %i.%i.%i.%i.",(sender.sin_addr.s_addr)&255,(sender.sin_addr.s_addr>>8)&255,(sender.sin_addr.s_addr>>16)&255,(sender.sin_addr.s_addr>>24)&255);
				outBuf[1] = 1;
				mConnected = true;
				mPingTimeout = 0;
				mPingDelay = 0;
				saout = sender;
				sendto(mSock,outBuf,2,0,(struct sockaddr*)&saout, sizeof(saout));
			} else {
				debugPrint("Connection rejected from %i.%i.%i.%i.",(sender.sin_addr.s_addr)&255,(sender.sin_addr.s_addr>>8)&255,(sender.sin_addr.s_addr>>16)&255,(sender.sin_addr.s_addr>>24)&255);
				outBuf[1] = 0;
				sendto(mSock,outBuf,2,0,(struct sockaddr*)&sender, sizeof(sender));
			}
		}
		else {
			inBuf[datalen]=0;
			handlePacket (inBuf[0], &inBuf[1], datalen-1);
		}
	}

	if (mConnected) {
		mPingDelay++;
		if (mPingDelay > PINGDELAY) {
			outBuf[0] = PING;
			sendto(mSock,outBuf,1,0,(struct sockaddr*)&saout, sizeof(saout));
			mPingDelay = 0;
		}
		mPingTimeout++;
		if (mPingTimeout > PINGTIMEOUT)
			disconnectClient (DISC_TIMEOUT);
	}
}

void CServer::handlePacket(BYTE command, char* data, int datalen) {
	unsigned short screenx,screeny;
	int x,y;

	switch (command) {
		case KEYPRESS:
//			if ((unsigned char) data[0] > 127) {
				sendVirtualKey ((unsigned char)data[0], true);
				sendVirtualKey ((unsigned char)data[0], false);
//			} else {
//				sendKey ((unsigned char)data[0], false, true);
//				sendKey ((unsigned char)data[0], false, false);
//			}
			break;
		case VKEYDOWN:
		case KEYDOWN:
			sendVirtualKey ((unsigned char)data[0], true);
			break;
		case VKEYUP:
		case KEYUP:
			sendVirtualKey ((unsigned char)data[0], false);
			break;
		case MOUSECOORD:
			x = (unsigned char)data[0];
			y = (unsigned char)data[1];
			x-=2; y-=2;
			if (x < 0) x = 0;
			else if (x > 252) x = 252;
			if (y < 0) y = 0;
			else if (y > 188) y = 188;

			setMouse (((unsigned char) x)*260, ((unsigned char) y)*351);
			break;
		case MOUSECOORDREL:
//			x = (unsigned char)data[0];
//			y = (unsigned char)data[1];
			
			if (lastMouseX == 0 && lastMouseY == 0) {
				lastMouseX = data[0];
				lastMouseY = data[1];
				break;
			}
			
			x = ((unsigned char) data[0])-lastMouseX;
			y = ((unsigned char) data[1])-lastMouseY;
			
			lastMouseX = data[0];
			lastMouseY = data[1];
			
			setMouse (x, y, true);
			break;
		case MOUSECOORDRELSTART:
			lastMouseX = data[0];
			lastMouseY = data[1];
			break;
		case MOUSECOORDZOOM:
			setMouse (((unsigned char)data[0])*257, ((unsigned char)data[1])*343, false);
			setMouse ((unsigned char)data[2], (unsigned char)data[3], true);
			break;
//		case MOUSECOORDZOOMSTART:
//			zoomX = data[0];
//			zoomY = data[1];
//			break;
//		case MOUSECLICK:
//			clickMouse();
			break;
		case MOUSECLICKDOWN:
			clickMouseDown();
			break;
		case MOUSECLICKUP:
			clickMouseUp();
			break;
//		case MOUSERCLICK:
//			clickRightMouse();
			break;
		case MOUSERCLICKDOWN:
			clickRightMouseDown();
			break;
		case MOUSERCLICKUP:
			clickRightMouseUp();
			break;
		case MOUSEMCLICKDOWN:
			clickMiddleMouseDown();
			break;
		case MOUSEMCLICKUP:
			clickMiddleMouseUp();
			break;
		case SCREENSHOT:
			sendScreenShot(true, 0, 0, &screenFull);
			break;
		case SCREENSHOTDATA:
			sendScreenBlock (data[0], &screenFull);
			break;
		case SCREENSHOTPART:
//			memcpy (&screenx, &data[0], 2);
//			memcpy (&screeny, &data[2], 2);
			getScreenRes(&screenx,&screeny);
			screenx = (screenx/256)*((unsigned char)data[0]);
			screeny = (screeny/192)*((unsigned char)data[1]);
			
			sendScreenShot(false, screenx, screeny, &screenPart);
			break;
		case SCREENSHOTPARTDATA:
			sendScreenBlock (data[0], &screenPart);
			break;
		case PING:
			mPingTimeout = 0;
			break;
		case DISCONNECT:
			disconnectClient(DISC_QUIT);
			break;
	}

}

void CServer::sendScreenBlock(unsigned char block, struct SCREENSHOTDATA* buffer) {
	unsigned int numRead;
	
	outBuf[0]=buffer->sendBlockCommand;
	outBuf[1]=block;
	
	for (numRead = 0; numRead < BLOCKSIZE && ((block*BLOCKSIZE)+numRead) < buffer->size; numRead++)
		outBuf[2+numRead] = buffer->screenBuf[(block*BLOCKSIZE)+numRead];

	if (numRead > 0) {
		sendto(mSock,outBuf,numRead+2,0,(struct sockaddr*)&saout, sizeof(saout));

		//debugPrint ("Sent screen block %i of %i bytes.", block, numRead);
	}
}

bool CServer::sendScreenShot(bool full, unsigned short x, unsigned short y, struct SCREENSHOTDATA* buffer) {
	unsigned char numBlocks;
	
	if (!screenCap(full, x, y))
		return false;

	if ((screen = fopen("ss.jpg", "rb")) == NULL)
		return false;

	buffer->size = fread (buffer->screenBuf, 1, 32768, screen);
	
	if (buffer->size > 30000)
		return false;

	fclose (screen);

	numBlocks = buffer->size/BLOCKSIZE;
	if (buffer->size > (numBlocks*BLOCKSIZE))
		numBlocks++;

	outBuf[0] = buffer->sendCommand;
	outBuf[1] = numBlocks;

//	debugPrint ("Sent screenshot init of %i bytes in %i blocks.", screenSize, numBlocks);

	sendto(mSock,outBuf,2,0,(struct sockaddr*)&saout, sizeof(saout));
	
	return true;
}

bool CServer::open(char* password) {
	int error;
	int nonblock = 1;

	if (mIsRunning) {
		debugPrint ("Server is already running.");	
		return false;
	}

	if (error = WSAStartup (MAKEWORD(2, 2), &wsaData) != 0) {
		debugPrint ("Stupid Winsock couldn't startup. Error: %i", error);
		return false;
	}
	
	mSock=socket(AF_INET,SOCK_DGRAM,0);

	if(bind(mSock,(struct sockaddr *)&sain,sizeof(sain)) != 0) {
		debugPrint("bind() error: %i. This probably means port 8888 is already in use.", WSAGetLastError());
		return false;
	}
	
	if (ioctlsocket(mSock, FIONBIO, (u_long FAR*) &nonblock) != 0) {
		debugPrint ("Set nonblocking failed.");
		return false;
	}
	

	debugPrint ("Server opened on port %i.", mPort);
	
	strcpy (mPassword, password);
	mIsRunning = true;
	return true;
}

void CServer::disconnectClient (int reason) {
	switch (reason) {
		case DISC_QUIT:
			debugPrint ("Client disconnected.");
			break;
		case DISC_TIMEOUT:
			debugPrint ("Client timed out.");
			break;
	}

	mConnected = false;
	saout.sin_addr.s_addr = 0;
}

void CServer::close() {
	mIsRunning = false;

	disconnectClient(DISC_SHUTDOWN);
	WSACleanup();

	debugPrint ("Server closed.");
}

CServer::~CServer(void)
{
}
