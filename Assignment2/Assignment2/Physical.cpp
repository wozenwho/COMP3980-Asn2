#include <Windows.h>
#include "Common.h"
#include "Header.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"


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
			TextOut(hdc, xPosition, yPosition, currTag, 50);
			//			TextOut(hdc, xPosition, yPosition + 20, temp, 50);

		}
	}
	return reading;
}



DWORD WINAPI ThreadProc(LPVOID v) {

	SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 0, 1, NULL);

	SkyeTek_FreeReaders(readers, numReaders);
	SkyeTek_FreeDevices(devices, numDevices);
	return 0;

}