//Init packet
//[PACKET][ID][NUMBLOCKS][SIZE][SIZE][SIZE][SIZE]
//Block packet
//[PACKET][ID][BLOCKNUM][CHKSUM][CHKSUM][DATA...]

#pragma once
#include <stdio.h>
#include "netdata.h"

class CFileSend
{
private:
	FILE* mFile;
	bool mOpen, mInitRecieved;
	unsigned char mID, mNumBlocks;
	unsigned short mLastBlockSize;
	unsigned long mFileSize;
	int retryDelay;
public:
	CFileSend(unsigned char ID);
	~CFileSend(void);
	bool open(const char* filename);
	bool isOpen() { return mOpen; }
	int getData(char* buffer);
};
