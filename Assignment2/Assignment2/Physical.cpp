#include <Windows.h>
#include <vector>
#include "Common.h"
#include "Header.h"
#include "Application.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

std::vector<char*> usedTags;

/*
SelectLoopCallback header
*/
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
{
	HDC hdc = GetDC(hwnd);
	char currTag[128];
	memset(currTag, ' ', 128);
	int currChar = 0;
	bool seenNull = false;

	if (lpTag != NULL) {
		for (int i = 0; i < 128; i++)
		{

			if (lpTag->friendly[i] == '\0') {
				if (seenNull) {
					currTag[currChar] = '\0';
					usedTags.push_back(currTag);
					break;
				}
				seenNull = true;
			}
			else
			{
				seenNull = false;
				currTag[currChar] = lpTag->friendly[i];
				currChar++;
			}
		}
	}
	//char * temp = (char*)lpTag->id;
	if (reading)
	{
		if (lpTag != NULL)
		{
			PrintTag(hwnd, currTag);

		}
	}
	return reading;
}

/*
ThreadProc header
*/
DWORD WINAPI ThreadProc(LPVOID v) {

	unsigned short localTags = 0;
	unsigned short previousTags;
	while (true) {
		previousTags = localTags;
		SkyeTek_GetTags(readers[0], AUTO_DETECT, &lpTags, &localTags);
		if (localTags != previousTags) {
			if (InvalidateRect(hwnd, &tagDisplayArea, TRUE)) {
				xPosTag = startingXPosTag;
				yPosTag = startingYPosTag;
			}
			SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 1, 0, NULL);
		}
		
	}
	SkyeTek_FreeReaders(readers, numReaders);
	SkyeTek_FreeDevices(devices, numDevices);
	return 0;
}