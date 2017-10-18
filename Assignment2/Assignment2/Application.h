#pragma once
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void PrintDevice(HWND, unsigned int);
void PrintTag(HWND, char*);