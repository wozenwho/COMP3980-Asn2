/*
HEADER
*/

#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

#include "Header.h"
#include "Common.h"
#include "Application.h"
#include "Physical.h"
#include "Session.h"


using namespace std;

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
int totalTags = 0;

int totalReads = 0;
unsigned int xPosDevice = startingXPosDevice;
unsigned int yPosDevice = startingYPosDevice;
unsigned int xPosTag = startingXPosTag;
unsigned int yPosTag = startingYPosTag;

BOOL reading = FALSE;

RECT tagDisplayArea = { tagDisplayArea.left = tagDisplayAreaLeft
					  , tagDisplayArea.top = tagDisplayAreaTop
					  , tagDisplayArea.right = tagDisplayAreaRight
					  , tagDisplayArea.bottom = tagDisplayAreaBottom };
RECT deviceDisplayArea;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ReadRFID(LPVOID);
void readTags(HWND);
void PrintDevice(HWND, unsigned int);
void PrintTag(HWND);

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

	hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10, windowWidth, windowHeight, NULL, NULL, hInst, NULL);
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
			ConnectDevice();
			break;
		case (MENU_START):
			// When START menu button is clicked
			// TODO: call StartReading() in Session.cpp
			if (reading) {
				break;
			}
			// TODO: Refactor, CreateThread in WinMain suspended, unsuspend in this case
			readThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)hwnd, 0, &threadId);
			reading = true;
			//WaitForSingleObject(readThread, 10000);
			break;
		case (MENU_STOP):
			// When STOP menu button is clicked
			// TODO: close handle(?) of RFID reader
			// TODO: call StopReading() in Session.cpp
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

/*
PrintDevice header
*/
void PrintDevice(HWND hwnd, unsigned int readFlag)
{
	PAINTSTRUCT paintstruct;
	HDC hdc = GetDC(hwnd);
	switch (readFlag)
	{
	case 0:
		for (int i = 0; i < numReaders; i++)
		{
			TextOut(hdc, xPosDevice, yPosDevice, readers[i]->friendly, 20);
			yPosDevice += yCoordOffset;
			TextOut(hdc, xPosDevice, yPosDevice, readers[i]->rid, 20);
			yPosDevice += yCoordOffset;
			TextOut(hdc, xPosDevice, yPosDevice, readers[i]->model, 20);
			yPosDevice += yCoordOffset;
			TextOut(hdc, xPosDevice, yPosDevice, readers[i]->manufacturer, 20);
			yPosDevice += yCoordOffset;
			TextOut(hdc, xPosDevice, yPosDevice, readers[i]->firmware, 20);
			yPosDevice += yCoordOffset;
		}
		break;
	case 1:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not discover reader", 30);
		break;
	case 2:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not detect Devices", 20);
		break;
	default:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not detect Devices", 20);
		break;
	}
}

/*
PrintTag header
*/
void PrintTag(HWND hwnd, char* currTag)
{
	HDC hdc = GetDC(hwnd);
	TextOut(hdc, xPosTag, yPosTag, currTag, 50);
	yPosTag += 20;
}