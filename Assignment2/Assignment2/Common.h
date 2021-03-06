#pragma once
#include <vector>
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

extern HWND hwnd;

extern HANDLE readThread;

extern LPSKYETEK_DEVICE* devices;
extern LPSKYETEK_READER* readers;

extern unsigned int numDevices;
extern unsigned int numReaders;

extern RECT tagDisplayArea;
extern RECT deviceDisplayArea;

extern unsigned int xPosTag;
extern unsigned int yPosTag;

extern BOOL reading;