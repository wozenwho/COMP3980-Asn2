#include <Windows.h>
#include "Header.h"
#include "Common.h"
#include "Session.h"
#include "SkyeTekAPI.h"
#include "Application.h"

/*
ConnectDevice header
*/
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