#include <Windows.h>
#include "Header.h"
#include "Common.h"
#include "Session.h"
#include "SkyeTekAPI.h"
#include "Application.h"


/*------------------------------------------------------------------------------------------------------------------
--  FUNCTION: ConnectDevice
--
--  DATE:			October 15, 2017
--
--  REVISIONS:		
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		void printDevice(HWND hwnd)
--						HWND hwnd: the handle for the application window
--
--  RETURNS:		returns a boolean value indicating if ConnectDevice successfully connects
--					to the RFID reader.
--
--  NOTES:
--	This function is called by WndProc when the 'Connect' menu button is pressed. 
--	It calls the SkyeTek API's SkyeTek_DiscoverDevices() and SkyeTek_DiscoverReaders() 
--	functions to detect and connect to the RFID reader. 
--
--	It calls PrintDevice() with a connection status flag to display the device's status
--  to the user. The connection status flag determines the message that is displayed.
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

/*------------------------------------------------------------------------------------------------------------------
--  FUNCTION: DisconnectDevice
--
--  DATE:			October 15, 2017
--
--  REVISIONS:		
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		void printDevice(HWND hwnd)
--						HWND hwnd: the handle for the application window
--
--  RETURNS:		void.
--
--  NOTES:
--	This function is called by WndProc when the 'Disconnect' menu button is pressed.
--	It gets the device context for the window to clear the device status' display area
--	and calls InvalidateRect to clear the device display area.	
--
--	It sets reading to false to stop readThread's execution.
----------------------------------------------------------------------------------------------------------------------*/
void DisconnectDevice(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	InvalidateRect(hwnd, &deviceDisplayArea, TRUE);
	reading = false;	
}

