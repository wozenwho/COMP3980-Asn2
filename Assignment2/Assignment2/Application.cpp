/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Application.cpp - An application that will connect to local RFID device to read nearby tags and diplsay tag info to the screen
--
-- PROGRAM: Assignment2
--
-- FUNCTIONS:
-- LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
--
-- DATE: October 15, 2017
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Matthew Shew, Wilson Hu
--
-- PROGRAMMER: Matthew Shew, Wilson Hu
--
-- NOTES:
-- The program will detect for local RFID devices and attempt to connect to the RFID device. The RFID will be set to read 
-- mode and will listen constantly for any nearby tags. When a tag comes in proximity of the RFID, the RFID will read the 
-- tag info and the program will print the tag info to the screen. When a user disconnects the RFID the program will 
-- release any handles to connected devices and be ready to connect to another RFID device when prompted. If a user selects 
-- "quit" the the program will release any handles to connected devices and the program will terminate closing the application window. 
--
----------------------------------------------------------------------------------------------------------------------*/


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

unsigned int numDevices;
unsigned int numReaders;

unsigned int xPosDevice = startingXPosDevice;
unsigned int yPosDevice = startingYPosDevice;
unsigned int xPosTag = startingXPosTag;
unsigned int yPosTag = startingYPosTag;

BOOL reading = FALSE;

RECT tagDisplayArea = { tagDisplayArea.left = tagDisplayAreaLeft
					  , tagDisplayArea.top = tagDisplayAreaTop
					  , tagDisplayArea.right = tagDisplayAreaRight
					  , tagDisplayArea.bottom = tagDisplayAreaBottom };
RECT deviceDisplayArea = { deviceDisplayArea.left = deviceDisplayAreaLeft
						 , deviceDisplayArea.top = deviceDisplayAreaTop
						 , deviceDisplayArea.right = deviceDisplayAreaRight
						 , deviceDisplayArea.bottom = deviceDisplayAreaBottom };




/*DWORD WINAPI ReadRFID(LPVOID);
void readTags(HWND);
void PrintDevice(HWND, unsigned int);
void PrintTag(HWND); */


/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: WinMain
--
--DATE : October 15, 2017
--
--REVISIONS : (Date and Description)
--
--DESIGNER : Matthew Shew, Wilson Hu
--
--PROGRAMMER : Matthew Shew, Wilson Hu
--
--INTERFACE : int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
HINSTANCE hInst: .
HINSTANCE hprevInstance: .
LPSTR lspszCmdParam: .
int nCmdShow: .
--
--RETURNS : int.
--
--NOTES :
--Call this function to begin the RFID application
----------------------------------------------------------------------------------------------------------------------*/
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


/*------------------------------------------------------------------------------------------------------------------
--FUNCTION: WndProc
--
--DATE : October 15, 2017
--
--REVISIONS : (Date and Description)
--
--DESIGNER : Matthew Shew, Wilson Hu
--
--PROGRAMMER : Matthew Shew, Wilson Hu
--
--INTERFACE : LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--				HWND hwnd: The handle to the window.
--				UINT Message: .
--				WPARAM wParam: The event that has occured.
--				LPARAM lParam: .
--
--
--RETURNS : LRESULT.
--
--NOTES :
--This function is called when a user event occurs
----------------------------------------------------------------------------------------------------------------------*/

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
			if (reading) {
				break;
			}
			if (ConnectDevice())
			{
				readThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)hwnd, 0, &threadId);
				reading = true;
			}
			break;
		case (MENU_DISCONNECT):
			// When START menu button is clicked
			DisconnectDevice(hwnd);
			break;
		case (MENU_QUIT):
			// When STOP menu button is clicked
			reading = false;
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &paintstruct);
		break;
	case WM_DESTROY:
		reading = false;
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

	//InvalidateRect(hwnd, &deviceDisplayArea, TRUE);

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
	yPosDevice = startingYPosDevice;
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
