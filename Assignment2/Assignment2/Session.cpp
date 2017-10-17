#include <Windows.h>
#include "Header.h"
#include "Common.h"
#include "Session.h"
#include "SkyeTekAPI.h"
#include "Application.h"


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
void ConnectDevice()
{
	if ((numDevices = SkyeTek_DiscoverDevices(&devices)) > 0)
	{
		if ((numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers)) > 0)
		{
			PrintDevice(hwnd, connectDeviceSuccess);
		}
		else {
			PrintDevice(hwnd, connectDeviceNoReader);
		}
	}
	else {
		PrintDevice(hwnd, connectDeviceNoDevice);
	}
	return true;
}


/*
StartReading header
*/
void StartReading() {
	//un-suspend thread
	//set reading = true
}

/*
StopReading header
*/
void StopReading()
{
	//suspend thread
	//set reading = false
}

