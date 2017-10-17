/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: SerialComm.c - An application that will communicate between 2 computers via serial port
--
-- PROGRAM: Assignment1
--
-- FUNCTIONS:
-- LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
-- DWORD WINAPI reading(LPVOID);
-- void setupMenu(HWND);
-- void read(HWND);
-- void updateSettings(WPARAM, HMENU);
-- void connect(HWND);
--
--
-- DATE: October 3, 2017
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Matthew Shew
--
-- PROGRAMMER: Matthew Shew
--
-- NOTES:
-- The program will create a connection between 2 computers using a serial port and user specified settings to open the port.
-- A thread will constantly listen for incoming charcters as the user types in characters on the keyboard to send to the other
-- computer. A user will begin in command mode where they can chose their port options and when ready, choose "connect mode" to
-- open the port and communicate with the other computer.
--
----------------------------------------------------------------------------------------------------------------------*/

#define STRICT
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <string.h>

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

LPSKYETEK_DEVICE *devices = NULL;
LPSKYETEK_READER *readers = NULL;

unsigned int numDevices;
unsigned int numReaders;

unsigned int xPosition = 0;
unsigned int yPosition = 0;

BOOL reading = FALSE;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


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
			if (connectDevice())
			{
				readThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)hwnd, 0, &threadId);
				reading = true;
			}
			break;
		case (MENU_DISCONNECT):
			// When START menu button is clicked
			reading = false;
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

