/*------------------------------------------------------------------------------------------------------------------
--  SOURCE FILE: Application.cpp - An application that will connect to local RFID device to read nearby tags and diplsay tag info to the screen
--
--  PROGRAM:	Assignment2
--
--  FUNCTIONS:
--				LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
--
--  DATE:		October 15, 2017
--
--  REVISIONS:	(Date and Description)
--
--  DESIGNER:	Matthew Shew, Wilson Hu
--
--  PROGRAMMER: Matthew Shew, Wilson Hu
--
--  NOTES:
--  The program will detect for local RFID devices and attempt to connect to the RFID device. The RFID will be set to read 
--  mode and will listen constantly for any nearby tags. When a tag comes in proximity of the RFID, the RFID will read the 
--  tag info and the program will print the tag info to the screen. When a user disconnects the RFID the program will 
--  release any handles to connected devices and be ready to connect to another RFID device when prompted. If a user selects 
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

LPSKYETEK_DEVICE* devices = NULL;
LPSKYETEK_READER* readers = NULL;
unsigned int numDevices;
unsigned int numReaders;
unsigned int xPosDevice = startingXPosDevice;
unsigned int yPosDevice = startingYPosDevice;
unsigned int xPosTag = startingXPosTag;
unsigned int yPosTag = startingYPosTag;
BOOL reading = FALSE;
/* RECT struct that holds coordinates to display tags' information
*/
RECT tagDisplayArea = { tagDisplayArea.left = tagDisplayAreaLeft
					  , tagDisplayArea.top = tagDisplayAreaTop
					  , tagDisplayArea.right = tagDisplayAreaRight
					  , tagDisplayArea.bottom = tagDisplayAreaBottom };
/* RECT struct that holds coordinates to display device information
*/
RECT deviceDisplayArea = { deviceDisplayArea.left = deviceDisplayAreaLeft
						 , deviceDisplayArea.top = deviceDisplayAreaTop
						 , deviceDisplayArea.right = deviceDisplayAreaRight
						 , deviceDisplayArea.bottom = deviceDisplayAreaBottom };

/*------------------------------------------------------------------------------------------------------------------
--  FUNCTION:	WinMain
--
--  DATE:			October 15, 2017
--
--  REVISIONS:
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
--						HINSTANCE hInst: the handle to the current instance of the program
--						HINSTANCE hprevInstance: the handle to the previous instance of the program
--						LPSTR lspszCmdParam: command line for the application
--						int nCmdShow: control parameter that determines how the window is shown
--
--  RETURNS:		Returns an int value based on function's success. 
--						Returns 0 if the window fails to register. 
--						Returns Msg.wParam otherwise. 
--
--  NOTES:
--  This function is the entry point into the program, and creates the window and menu for the program.
--  It contains the core messaging loop that runs for the program's lifetime. 
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
--  FUNCTION: WndProc
--
--  DATE:			October 15, 2017
--
--  REVISIONS:		
--
--  DESIGNER:		Matthew Shew, Wilson Hu
--
--  PROGRAMMER:		Matthew Shew, Wilson Hu
--
--  INTERFACE:		LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--						HWND hwnd: the handle to the window
--						UINT Message: the message generated by an event
--						WPARAM wParam: additional message information
--						LPARAM lParam: additional message information
--
--
--  RETURNS:		LRESULT.
--
--  NOTES:
--  This function is called when a Windows event occurs. This function processes the message and 
--	executes one case based on the message type. The messages it processes are: menu button clicks, 
--	paint, and destroy. The menu buttons are used to connect or disconnect the device, and to quit the
--	program. The paint message is called to repaint different areas of the window (which are initialized
--	in the RECT structures), and the destroy message closes the program.
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
			DisconnectDevice(hwnd);
			break;
		case (MENU_QUIT):
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

/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION: PrintDevice
--
--	DATE:			October 15, 2017
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNER:		Matthew Shew, Wilson Hu
--
--	PROGRAMMER:		Matthew Shew, Wilson Hu
--
--	INTERFACE:		void PrintDevice(HWND hwnd, unsigned int readFlag)
--						HWND hwnd: handle to the window that PrintDevice will print to
--						unsigned int readFlag: int flag that ConnectDevice uses to indicate connection status
--												flags are defined in Header.h
--
--	RETURNS:		void
--
--	NOTES:
--	This function is called by ConnectDevice(). It prints a message to the window that displays the 
--	reader's status. The readFlag parameter is used to print a corresponding status message
--  (success or fail) based on ConnectDevice's success.
----------------------------------------------------------------------------------------------------------------------*/
void PrintDevice(HWND hwnd, unsigned int readFlag)
{
	PAINTSTRUCT paintstruct;
	HDC hdc = GetDC(hwnd);

	switch (readFlag)
	{
	case connectDeviceSuccess:
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
	case connectDeviceNoReader:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not discover reader", 30);
		break;
	case connectDeviceNoDevice:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not detect Devices", 20);
		break;
	default:
		TextOut(hdc, xPosDevice, yPosDevice, "Could not detect Devices", 20);
		break;
	}
	yPosDevice = startingYPosDevice;
}

/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION: PrintTag
--
--	DATE:		October 15, 2017
--
--	REVISIONS:	(Date and Description)
--
--	DESIGNER:	Matthew Shew, Wilson Hu
--
--	PROGRAMMER: Matthew Shew, Wilson Hu
--
--	INTERFACE:	void PrintTag(HWND hwnd, char* currTag)
--					HWND hwnd: the handle to the window
--					char* currTag: char pointer to a string containing a tag's data
--
--	RETURNS:	void
--
--	NOTES:
--	This function is called by SelectLoopCallback. It is called in a loop to print each tag detected by the reader.
--  The function prints the string stored in currTag to the window given by the handle hwnd. This function increases
--  yPosTag by yCoordOffset each time to avoid overlapping messages. Each message is printed below the previous.
----------------------------------------------------------------------------------------------------------------------*/
void PrintTag(HWND hwnd, char* currTag)
{
	HDC hdc = GetDC(hwnd);
	TextOut(hdc, xPosTag, yPosTag, currTag, 50);
	yPosTag += yCoordOffset;
}
