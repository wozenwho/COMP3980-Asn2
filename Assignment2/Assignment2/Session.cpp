#include <Windows.h>
#include "Common.h"
#include "SkyeTekAPI.h"


void printDevice(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT paintstruct;
	hdc = GetDC(hwnd);

	TextOut(hdc, 0, 0, "called printDevice()", 20);

	//TODO: add error message
	if ((numDevices = SkyeTek_DiscoverDevices(&devices)) > 0)
	{
		if ((numReaders = SkyeTek_DiscoverReaders(devices, numDevices, &readers)) > 0)
		{
			for (ix = 0; ix < numReaders; ix++)
			{
				TextOut(hdc, xPosition, yPosition, readers[ix]->friendly, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[ix]->rid, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[ix]->model, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[ix]->manufacturer, 50);
				yPosition += 20;
				TextOut(hdc, xPosition, yPosition, readers[ix]->firmware, 50);
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

/*
void readTags(HWND hwnd)
{
HDC hdc = GetDC(hwnd);

//SkyeTek_SetAdditionalTimeout(readers[0]->lpDevice, 500);

while (reading)
{
lpTags = NULL;
count = 0;
st = SkyeTek_GetTags(readers[0], AUTO_DETECT, &lpTags, &count);
if (st == SKYETEK_TIMEOUT)
{
TextOut(hdc, xPosition, yPosition, "SKYETEK TAGS TIMED OUT.", 20);
yPosition += yCoordOffset;
}
else if (st != SKYETEK_SUCCESS)
{
TextOut(hdc, xPosition, yPosition, "SKYETEK_GETTAGS FAILED.", 20);
}

for (size_t i = 0; i < count; i++)
{
TextOut(hdc, xPosition, yPosition, lpTags[i]->friendly, 50);
yPosition += yCoordOffset;
TextOut(hdc, xPosition, yPosition, SkyeTek_GetTagTypeNameFromType(lpTags[i]->type), 50);
yPosition += yCoordOffset;
}
}
}

*/