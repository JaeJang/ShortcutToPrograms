#pragma once

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#define MAX_WINDOW_NAME 1024

#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>
#include <string.h>
#include <list>
#include <deque>


#pragma comment(lib,"User32.lib")

#define LIST std::list<HWND>&
#define PAIR std::pair<STRING, LIST>

typedef std::wstring STRING;


class EnumOpenWindows {
public:
	static BOOL GetOpenProcess(LIST list, STRING path);

private:
	static BOOL CALLBACK EnumWindowsCallBack(HWND hwnd, LPARAM lParam);
	static BOOL IsAltTabWindow(const HWND hwnd);
	static STRING GetProcessName(const HWND & hwnd);
	static void RemoveClosedWindow(LIST);
};