/*
HEADER
*/

#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <string.h>

#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

#include "Header.h"

char Name[] = "C3980 Asn 2";
HWND hwnd;


HANDLE readThread;
char inputBuffer[512];

LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;
LPSKYETEK_TAG *lpTags = NULL;
LPSKYETEK_DATA lpData = NULL;
SKYETEK_STATUS st;
unsigned short count;
unsigned int numDevices;
unsigned int numReaders;
int ix = 0;
int loops = 100;
int totalReads = 0;
int failedReads = 0;
int failedLoops = 0;
unsigned int xPosition = 0;
unsigned int yPosition = 0;

BOOL reading = FALSE;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ReadRFID(LPVOID);
void printDevice(HWND);
void readTags(HWND);
DWORD WINAPI ThreadProc(LPVOID);

unsigned char SelectLoopCallback(LPSKYETEK_TAG, void *);

/*
WinMain() header
*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;
	PAINTSTRUCT paintstruct;
	HDC hdc = BeginPaint(hwnd, &paintstruct);


	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wcl.hIconSm = NULL;
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = "CommandMenu";
	Wcl.cbClsExtra = 0;
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10, 800, 600, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);


	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

/*
WndProc() Header
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT paintstruct;
	DWORD threadId;

	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case (MENU_CONNECT):
			// When CONNECT menu button is clicked
			printDevice(hwnd);
			break;
		case (MENU_START):
			// When START menu button is clicked
			reading = true;
			readThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)hwnd, 0, &threadId);
			break;
		case (MENU_STOP):
			// When STOP menu button is clicked
			// TODO: close handle(?) of RFID reader
			reading = FALSE;
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintstruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

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






unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
{

	HDC hdc = GetDC(hwnd);
	if (reading)
	{
		if (lpTag != NULL)
		{
			TextOut(hdc, xPosition, yPosition, lpTag->friendly, 50);
			yPosition += 20;
		}
	}
	return(reading);
}



DWORD WINAPI ThreadProc(LPVOID v) {

	SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 0, 1, NULL);
	

	SkyeTek_FreeReaders(readers, numReaders);
	SkyeTek_FreeDevices(devices, numDevices);
	return 0;

}