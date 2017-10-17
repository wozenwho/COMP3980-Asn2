#pragma once
#include <vector>
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

extern HWND hwnd;

extern HANDLE readThread;


extern LPSKYETEK_DEVICE* devices;
extern LPSKYETEK_READER* readers;
extern LPSKYETEK_TAG* lpTags;


extern unsigned int numDevices;
extern unsigned int numReaders;

extern int totalTags;

extern RECT tagDisplayArea;

extern int loops;
extern int totalReads;
extern unsigned int xPosTag;
extern unsigned int yPosTag;

extern BOOL reading;