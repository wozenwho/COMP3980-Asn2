#pragma once
#include <Windows.h>


//Print functions, menus?
//WinMain, WndProc
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void PrintDevice(HWND, unsigned int);
void PrintTag(HWND, char*);