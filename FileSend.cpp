#include ".\filesend.h"

CFileSend::CFileSend(unsigned char ID) {
	mID = ID;
	retryDelay = 0;
	mOpen = mInitRecieved = false;
	mFileSize = 0;
}

CFileSend::~CFileSend(void) {
}


//Update/fill send buffer if there is data to send
int CFileSend::getData(char* buffer) {
	int retVal = 0;
	
	if (!mInitRecieved) {
		if (retryDelay <= 0) {
			buffer[0] = FILESENDINIT;
			buffer[1] = mID;
			buffer[2] = mNumBlocks;
			//buffer[3] = (unsigned long) mFileSize;
//			memcpy (&buffer[3], &mFileSize, 4);

			retVal = 7;
			retryDelay = RETRYDELAY;
		} else {
			retryDelay--;
		}
	}

	return retVal;
}

bool CFileSend::open(const char* filename) {
	int wholeBlocks, partialBlock;

	if ((mFile = fopen(filename, "rb")) == NULL)
		return false;
	
	fseek (mFile , 0 , SEEK_END);
	mFileSize = (unsigned long) ftell (mFile);
	rewind (mFile);

	wholeBlocks = (int) mFileSize/BLOCKSIZE;
	if ((partialBlock = (mFileSize-(wholeBlocks*BLOCKSIZE))) > 0) {
		mNumBlocks = wholeBlocks+1;
		mLastBlockSize = partialBlock;
	} else {
		mLastBlockSize = 0;
	}

	mOpen = true;
	return true;
}