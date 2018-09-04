#pragma once

#include "BaseWindow.h"
#include "TextWindow.h"
#include "EditWindow.h"
#include "EnumOpenWindows.h"
#include "resource5.h"
#include <shellapi.h>
#include <fstream>
#include <map>
#include <string>
#include <wchar.h>
#include <list>
#include <deque>


#ifndef UNICODE
#define UNICODE
#endif

#define TRAY_ID 9000
#define TRAY_MENU_EXIT 8000

#define WM_TRAY (WM_USER + 1)

#define REGISTER_BUTTON 7000
#define SET_STARTUP 6000
#define REGISTRY_NAME L"ShortCut"

#define OFFSET 96
#define LOCK_ON 0

#define NUM_OF_HOT_KEY 10
#define TEXT_WINDOW_WIDTH 150
#define TEXT_WINDOW_HEIGHT 20
#define TEXT_WINDOW_X  50
#define TEXT_WINDOW_Y 60
#define TEXT_WINDOW_Y_INCREMENT 50

#define PATH_WINDOW_WIDTH 600
#define PATH_WINDOW_HEIGHT 20
#define PATH_WINDOW_X 200
#define PATH_WINDOW_Y TEXT_WINDOW_Y
#define PATH_WINDOW_Y_INCREMENT 50

#define REGISTER_BUTTON_X 850
#define REGISTER_BUTTON_Y 50
#define REGISTER_BUTTON_WIDTH 200
#define REGISTER_BUTTON_HEIGTH 100
#define MAX_WINDOW_NAME 1024

#define CHECKBOX_X 850
#define CHECKBOX_Y 230
#define CHECKBOX_WIDTH 200
#define CHECKBOX_HEIGHT 100

#define MESSAGE_BOX(message) MessageBoxW(m_hwnd, message, L"Program Shrotcut", MB_OK)
#define MESSAGE_BOX_ERROR(message) MessageBoxW(m_hwnd, message, L"Program Shrotcut", MB_OK | MB_ICONERROR)
#define ERROR_MESSAGE(msg, code) STRING s(msg);s+=L"(error code: ";s += std::to_wstring(code);s += L").";MESSAGE_BOX_ERROR(s.c_str())

typedef std::wstring STRING;
typedef std::list<HWND> LIST_HWND;

class MainWindow : public BaseWindow<MainWindow>
{
public:
	
	~MainWindow();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL Create(PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	);

	LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	PCWSTR ClassName() const;
	void CleanUp();
	void Minimize();

private:
	NOTIFYICONDATA notifyIconData;
	HMENU m_menu;

	//BaseWindow<TextWindow> *numPad;
	BaseWindow<EditWindow> *editWindows;
	std::map<size_t, WCHAR*> pathMap;
	std::map<STRING, LIST_HWND> openList;

	
	void InitNotifyIconData();
	
	void Restore();
	void AddContents(HWND hwnd);

	BOOL RegisterKeys();
	BOOL SetRegistryStartProgram(BOOL autoExec);

	void SavePrograms();
	BOOL LoadPrograms();
};

