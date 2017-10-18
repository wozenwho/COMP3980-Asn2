#include <Windows.h>
#include <vector>
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"
#include "Common.h"
#include "Header.h"
#include "Physical.h"
#include "Application.h"


/*------------------------------------------------------------------------------------------------------------------
--  FUNCTION: SelectLoopCallBack
--
--  DATE:			October 15, 2017
--
--  REVISIONS:		(Date and Description)
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
						LPSKYETEK_TAG lpTag: The handle for a nearby tag
						void* user: void pointer that can be used to pass user data to be called back
										at a later time. This field is not used in this function.
--
--  RETURNS:		unsigned char reading, the current state of the program (reading or not reading)
--
--  NOTES:
--  This function is called by the SkyeTek API SkyeTek_SelectTags function. It is called for each tag in
--  the reader's vicinity. The function processes the tag friendly field to remove null characters
--  and calls PrintTag to display the tag to the window.
----------------------------------------------------------------------------------------------------------------------*/
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
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
			PrintTag(hwnd, tag);
		}
	}
	return reading;
}

/*------------------------------------------------------------------------------------------------------------------
--  FUNCTION: ThreadProc
--
--  DATE:			October 15, 2017
--
--  REVISIONS:		(Date and Description)
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		DWORD WINAPI ThreadProc(LPVOID v)
						LPVOID v: Null for this application
--
--  RETURNS:		Returns 0 when the function ends. 
--
--  NOTES:
--  This function is called when a thread is created to set the RFID device to read mode to listen for nearby tags. 
--	While the program is in read mode, it continuously reads for nearby tags. Each iteration of the read loop compares
--	the current number of tags read to the previous number. If the number of nearby tags changes, it stores each tag
--	in lpTags and calls the SkyeTek_SelectTags API function to process each nearby tag.
--
--	When reading is set to false, the loop terminates and the function frees the connected readers and devices.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI ThreadProc(LPVOID v) {

	unsigned short localTags = 0;
	unsigned short previousTags;
	LPSKYETEK_TAG* lpTags;

	while (reading)
	{
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
