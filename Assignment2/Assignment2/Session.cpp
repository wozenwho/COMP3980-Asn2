#include <Windows.h>
#include "Common.h"
#include "SkyeTekAPI.h"

/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: connectDevice
--
--DATE : October 15, 2017
--
--REVISIONS : (Date and Description)
--
--DESIGNER : Matthew Shew, Wilson Hu
--
--PROGRAMMER : Matthew Shew, Wilson Hu
--
--INTERFACE : void printDevice(HWND hwnd)
HWND hwnd: the handle for the application window
--
--RETURNS : void.
--
--NOTES :
--This function is called to print the RFID device information to the screen
----------------------------------------------------------------------------------------------------------------------*/
void connectDevice()
{
	HDC hdc;
	PAINTSTRUCT paintstruct;
	hdc = GetDC(hwnd);

	TextOut(hdc, 0, 0, "called connectDevice()", 20);

	//TODO: add error message
	if ((numDevices = SkyeTek_DiscoverDevices(&devices)) > 0)
	{
		if ((numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers)) > 0)
		{
			for (int i = 0; i < numReaders; i++)
			{
				TextOut(hdc, xPosition, yPosition, readers[i]->friendly, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[i]->rid, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[i]->model, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[i]->manufacturer, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[i]->firmware, 50);
				yPosition += 20;
			}
		}
		else {
			TextOut(hdc, xPosition, yPosition, "Could not discover reader", 30);
		}
	}
	else {
		TextOut(hdc, xPosition, yPosition, "Could not detect Devices", 20);
	}
}

