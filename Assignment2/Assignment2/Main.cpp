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
#include "Physical.h"
#include "Session.h"
#include "Common.h"

using namespace std;

char Name[] = "C3980 Asn 2";
HWND hwnd;


HANDLE readThread;
char inputBuffer[512];

LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;
LPSKYETEK_TAG *lpTags = NULL;
vector<char *> usedTags;

LPSKYETEK_DATA lpData = NULL;
SKYETEK_STATUS st;
unsigned short count;
unsigned int numDevices;
unsigned int numReaders;
int totalTags = 0;



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
			if (reading) {
				break;
			}
			readThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)hwnd, 0, &threadId);
			reading = true;
			//WaitForSingleObject(readThread, 10000);
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

