#pragma once
#include <vector>
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

extern char Name[];
extern HWND hwnd;


extern HANDLE readThread;
extern char inputBuffer[512];

extern LPSKYETEK_DEVICE *devices;
extern LPSKYETEK_READER *readers;
extern LPSKYETEK_TAG *lpTags;
extern std::vector<char*> usedTags;

extern LPSKYETEK_DATA lpData;
extern SKYETEK_STATUS st;
extern unsigned short count;
extern unsigned int numDevices;
extern unsigned int numReaders;
extern int totalTags;



extern int ix;
extern int loops;
extern int totalReads;
extern int failedReads;
extern int failedLoopsextern;
extern unsigned int xPosition;
extern unsigned int yPosition;

extern BOOL reading;