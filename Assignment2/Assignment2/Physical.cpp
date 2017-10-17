#include <Windows.h>
#include "Common.h"
#include "Header.h"
#include "Physical.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"


/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: SelectLoopCallBack
--
--DATE : October 15, 2017
--
--REVISIONS : (Date and Description)
--
--DESIGNER : Matthew Shew, Wilson Hu
--
--PROGRAMMER : Matthew Shew, Wilson Hu
--
--INTERFACE : unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
LPSKYETEK_TAG lpTag: The handle for a nearby tag
void *user: user
--
--RETURNS : unsigned char.
--
--NOTES :
--This Function is called when the SKYTEK
----------------------------------------------------------------------------------------------------------------------*/
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
{
	HDC hdc = GetDC(hwnd);
	char tag[50];
	int tagSize = 0;
	bool seenNull = false;
	memset(tag, ' ', 50);

	if (lpTag != NULL) {
		for (int i = 0; i < 50; i++)
		{
			if (lpTag->friendly[i] == '\0') {
				if (seenNull) {
					tag[tagSize] = '\0';
					break;
				}
				seenNull = true;
			}
			else
			{
				seenNull = false;
				tag[tagSize] = lpTag->friendly[i];
				tagSize++;
			}
		}
	}

	if (reading)
	{
		if (lpTag != NULL)
		{
			TextOut(hdc, xPosition, yPosition, tag, 50);
			yPosition += 20;

		}
	}
	return reading;
}



/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: ThreadProc
--
--DATE : October 15, 2017
--
--REVISIONS : (Date and Description)
--
--DESIGNER : Matthew Shew, Wilson Hu
--
--PROGRAMMER : Matthew Shew, Wilson Hu
--
--INTERFACE : DWORD WINAPI ThreadProc(LPVOID v)
LPVOID v: Null for this application
--
--RETURNS : DWORD.
--
--NOTES :
--This function is called when a thread is created to set the RFID device to read mode to listen for nearby tags
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI ThreadProc(LPVOID v) {

	unsigned short localTags = 0;
	unsigned short previousTags;
	LPSKYETEK_TAG *lpTags;

	while (reading)
	{
		previousTags = localTags;
		SkyeTek_GetTags(readers[0], AUTO_DETECT, &lpTags, &localTags);
		if (localTags != previousTags)
		{
			SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 1, 0, NULL);
		}
	}
	SkyeTek_FreeReaders(readers, numReaders);
	SkyeTek_FreeDevices(devices, numDevices);
	return 0;

}