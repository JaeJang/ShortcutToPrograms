#ifndef UNICODE
#define UNICODE
#endif

//#define OPEN_WINDOW

#define MUTEX_NAME L"DO NOT CREATE INSTANTIATE THIS APP MORE THAN ONCE"
#define WINDOW_NAME L"Shortcut To Your Program"

#include <iostream>
#include "MainWindow.h"

void PrivilegeProcess();
HANDLE hMutex;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//PrivilegeProcess();
	MainWindow win;

	hMutex = CreateMutex(NULL, TRUE, MUTEX_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND fore = FindWindow(win.ClassName(), WINDOW_NAME);
		ShowWindow(fore,SW_SHOW);
		return 0;
	}

	if (!win.Create(WINDOW_NAME, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

#ifndef OPEN_WINDOW
	win.Minimize();
#endif

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}

void PrivilegeProcess() {
	BOOL SetPrivilege(HANDLE, LPCTSTR, BOOL);


	HANDLE hToken;
	DWORD err;
	if (!OpenThreadToken(GetCurrentThread(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		FALSE, &hToken))
	{
		err = GetLastError();
		if (err == ERROR_NO_TOKEN) {
			if (!ImpersonateSelf(SecurityImpersonation)) {
				err = GetLastError();
				std::cout << "Error ImpersonateSelf: " << err << std::endl;
				return;
			}
			if (!OpenThreadToken(GetCurrentThread(),
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
				FALSE, &hToken)) {
				std::cout << "Error OpenThreadToken: " << err << std::endl;
				return;
			}
		}
		else {
			std::cout << "Error OpenThreadToken(another): " << err << std::endl;
			return;
		}
	}

	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
	{
		CloseHandle(hToken);
		return;
	}
	if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE))
	{
		CloseHandle(hToken);
		return;
	}
}


BOOL SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);
	DWORD err;

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious
	);
	if ((err = GetLastError()) != ERROR_SUCCESS) {
		std::cout << "Error SetPrivilege1: " << err << std::endl;
		return FALSE;
	}
	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL
	);

	if ((err = GetLastError()) != ERROR_SUCCESS) {
		std::cout << "Error SetPrivilege2: " << err << std::endl;
		return FALSE;
	}
	return TRUE;
}