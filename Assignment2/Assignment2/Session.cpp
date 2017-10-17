#include <Windows.h>
#include "Header.h"
#include "Common.h"
#include "Session.h"
#include "SkyeTekAPI.h"
#include "Application.h"


/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: ConnectDevice
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
boolean ConnectDevice()
{
	if ((numDevices = SkyeTek_DiscoverDevices(&devices)) > 0)
	{
		if ((numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers)) > 0)
		{
			PrintDevice(hwnd, connectDeviceSuccess);
			return true;
		}
		else {
			PrintDevice(hwnd, connectDeviceNoReader);
			return false;
		}
	}
	else {
		PrintDevice(hwnd, connectDeviceNoDevice);
		return false;
	}
	return false;
}

/*
StopReading header
*/
void DisconnectDevice(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	InvalidateRect(hwnd, &deviceDisplayArea, TRUE);
	reading = false;	
	TextOut(hdc, startingXPosDevice, startingYPosDevice, "Disconnected device", 20);
}

