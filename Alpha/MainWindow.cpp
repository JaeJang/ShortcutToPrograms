#pragma once

#include "MainWindow.h"

#ifndef UNICODE
#define UNICODE
#endif

//#define EXIT_WITH_ESC

//Destructor
MainWindow::~MainWindow()
{
	CleanUp();
}

LRESULT MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MainWindow *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
		pThis = (MainWindow*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hwnd = hwnd;
	}
	else
	{
		pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	if (pThis)
	{
		return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

//Create main window
//RETURN: true if main window is created
BOOL MainWindow::Create(PCWSTR lpWindowName,
	DWORD dwStyle,
	DWORD dwExStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu)
{
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = MainWindow::WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = ClassName();

	RegisterClass(&wc);
	m_hwnd = CreateWindowEx(
		dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
		nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL),
		this
	);
	
	InitNotifyIconData();
	return (m_hwnd ? TRUE : FALSE);
}

PCWSTR MainWindow::ClassName() const
{
	return L"Shortcut To Your Programs";
}

//Delete an icon from the status area When the program is destoryed
void MainWindow::CleanUp()
{
	if (!IsWindowVisible(m_hwnd))
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	}

	SavePrograms();
	//delete[] numPad;
	delete[] editWindows;
	for (auto iter = pathMap.begin(); iter != pathMap.end(); ++iter)
		delete[] iter->second;
}

//Initilize NOTIFYICONDATA
void MainWindow::InitNotifyIconData()
{
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));

	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);

	//get a handle that receives notification associated
	//with and icon in the notification area
	//which is Main Window
	notifyIconData.hWnd = m_hwnd;

	//The application-defined identifier of the taskbar icon
	notifyIconData.uID = TRAY_ID;

	//Set up Flags
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	//Message Id for icon in the notification area
	notifyIconData.uCallbackMessage = WM_TRAY;

	//Set up icon image
	notifyIconData.hIcon = (HICON)LoadImage(NULL, L"Icon_tray.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	//notifyIconData.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON2));
	//Set up text when the mouse cursor is over the icon
	wcscpy_s(notifyIconData.szTip,128, L"test");
}

//Minimize the window to the notification area
void MainWindow::Minimize()
{
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);
	
	ShowWindow(m_hwnd, SW_HIDE);
}

//Restore the window to the foreground 
void MainWindow::Restore()
{
	Shell_NotifyIcon(NIM_DELETE, &notifyIconData);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
}

//Add all components in the window
void MainWindow::AddContents(HWND hwnd) {

	//numPad = new TextWindow[NUM_OF_HOT_KEY];
	editWindows = new EditWindow[NUM_OF_HOT_KEY];

	//memset(numPad, 0, sizeof(TextWindow) * NUM_OF_HOT_KEY);
	memset(editWindows, 0, sizeof(EditWindow) * NUM_OF_HOT_KEY);
	for (int i = 0; i < NUM_OF_HOT_KEY; ++i) {
		
		//Create labels that indicates keys needed to be pressed
		/*numPad[i] = */TextWindow(i,
			NULL, 
			WS_VISIBLE | WS_CHILD | SS_CENTER, 
			TEXT_WINDOW_X, TEXT_WINDOW_Y + (TEXT_WINDOW_Y_INCREMENT*i),
			TEXT_WINDOW_WIDTH, TEXT_WINDOW_HEIGHT, 
			m_hwnd);

		//Create text areas for paths
		editWindows[i] = EditWindow(i,
			WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER | ES_AUTOHSCROLL, 
			PATH_WINDOW_X, PATH_WINDOW_Y + (PATH_WINDOW_Y_INCREMENT * i),
			PATH_WINDOW_WIDTH, PATH_WINDOW_HEIGHT, 
			m_hwnd);
	}

	//Button for registering Hotkeys
	CreateWindowW(L"button", L"Register", 
		WS_VISIBLE | WS_CHILD | WS_BORDER, 
		REGISTER_BUTTON_X, REGISTER_BUTTON_Y, 
		REGISTER_BUTTON_WIDTH, REGISTER_BUTTON_HEIGTH,
		m_hwnd, 
		(HMENU)REGISTER_BUTTON, 
		NULL, NULL);

	//Button for automatic start
	CreateWindowW(L"button", L"Start automatically when \nWindows starts up",
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE,
		CHECKBOX_X, CHECKBOX_Y,
		CHECKBOX_WIDTH, CHECKBOX_HEIGHT,
		m_hwnd, (HMENU)SET_STARTUP, NULL, NULL);
	
}

//Register Hotkeys with the paths that user specified
//RETURN: true if all hotkeys are registered successfully
BOOL MainWindow::RegisterKeys()
{
	//Unregister hotkeys first
	for (auto iter = pathMap.begin(); iter != pathMap.end(); ++iter) {
		UnregisterHotKey(m_hwnd, iter->first);
	}
	pathMap.clear();
	openList.clear();

	//Get path from EditWindow
	//Put a key which will be pressed and path into map
	//so we can use the path when the key is actually pressed
	//Create a list as a value and path a key for openList Map
	//This map helps switching between windows that have
	//same path
	for (int i = 0; i < NUM_OF_HOT_KEY; ++i) {
		HWND tmp = editWindows[i].Window();
		WCHAR *path = new WCHAR[512];
		if (GetWindowText(tmp, path, 512)) {
			if (!RegisterHotKey(m_hwnd, i, MOD_WIN, i + OFFSET))
				return FALSE;
			pathMap.insert(std::pair<size_t, WCHAR*>(i, path));
			LIST_HWND d;
			openList.insert(std::pair < STRING, LIST_HWND>(STRING(path), d));
		}

	}
	return TRUE;
}

//PARAM	: if true, register this program as a startup program.
//		  If false, delete this program from registry.
//RETURN: true if the process is ended successfully
BOOL MainWindow::SetRegistryStartProgram(BOOL autoExec)
{
	WCHAR winName[MAX_WINDOW_NAME];
	GetWindowModuleFileName(m_hwnd, winName, MAX_WINDOW_NAME);

	HKEY hKey;
	DWORD d = 0;
	if (autoExec) 
	{
		d = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			NULL);
		if ( d== ERROR_SUCCESS) 
		{
			if ((d = RegSetValueEx(hKey,
				REGISTRY_NAME,
				0,
				REG_SZ,
				(BYTE*)winName,
				(lstrlen(winName)+1) * sizeof(WCHAR))) != ERROR_SUCCESS) 
			{
				ERROR_MESSAGE(L"Fail to set regestry", d);
				return FALSE;
			}
			
		}
		else if (d == ERROR_ACCESS_DENIED) {
			ERROR_MESSAGE(L"Access denied.\nPlease open as Administrator.", d);
			return FALSE;
		}
		else {
			ERROR_MESSAGE(L"Fail to open Registry", d);
			return FALSE;
		}


	}
	else
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			KEY_ALL_ACCESS,
			&hKey) == ERROR_SUCCESS)
		{
			d = RegDeleteValue(hKey, REGISTRY_NAME);
			if (d != ERROR_SUCCESS && d != ERROR_FILE_NOT_FOUND) {
				ERROR_MESSAGE(L"Can't delete from Start Program Registry", d);
				return FALSE;
			}
		}
		else {
			ERROR_MESSAGE(L"Fail to open Registry", d);
			return FALSE;
		}
	}
	RegCloseKey(hKey);
	return TRUE;
}

//The paths are automatically saved when the program is destroyed
void MainWindow::SavePrograms()
{
	WCHAR winName[MAX_WINDOW_NAME];
	GetWindowModuleFileName(m_hwnd, winName, MAX_WINDOW_NAME);
	STRING rootPath(winName);
	UINT found = rootPath.find_last_of(L"\\");
	rootPath = rootPath.substr(0, found + 1);
	rootPath += L"list.dat";

	std::wofstream fout;
	fout.open(rootPath);
	
	for (auto iter = pathMap.begin(); iter != pathMap.end(); ++iter) {
		fout << iter->first << "," << iter->second << L"\n";
	}
	fout.close();

}

//Load paths saved when this program closed lately
//RETURN: true if there is a save file.
BOOL MainWindow::LoadPrograms()
{
	//If the programs is registered in Registry,
	//checkbox should be checked.
	/******************************/
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_QUERY_VALUE,
		&hKey) == ERROR_SUCCESS) {
		if (RegQueryValueEx(hKey, REGISTRY_NAME, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
			CheckDlgButton(m_hwnd, SET_STARTUP, BST_CHECKED);
		else
			CheckDlgButton(m_hwnd, SET_STARTUP, BST_UNCHECKED);
	}
	/******************************/

	//I tried to use just "list.dat" for saved data,
	//but when the program is released and moved to another directory,
	//it doesn't read the file.
	//Instead, it gets full path to exe file and add the saved file name 
	//at the end of the path.
	WCHAR winName[MAX_WINDOW_NAME];
	GetWindowModuleFileName(m_hwnd, winName, MAX_WINDOW_NAME);
	STRING rootPath(winName);
	UINT found = rootPath.find_last_of(L"\\");
	rootPath = rootPath.substr(0, found + 1);
	rootPath += L"list.dat";

	std::wfstream fin;
	fin.open(rootPath);
	if (!fin) {
		ERROR_MESSAGE(L"fail to open file.\n Please place \"list.dat\" file in same folder.", errno);
		return FALSE;
		
	}
	WCHAR buffer[1024];


	while (fin.getline(buffer, 1024)) {
		WCHAR *buffer2;
		WCHAR *t = wcstok_s(buffer, L",", &buffer2);
		int i = wcstol(t, 0, 10);
		SetWindowText(editWindows[i].Window(), buffer2);
	}
	fin.close();
	
	return TRUE;

}



LRESULT MainWindow::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == 0 && !IsWindowVisible(m_hwnd)) {
		Minimize();
		return 0;
	}

	switch (uMsg)
	{

	case WM_CREATE:
	{
		AddContents(hwnd);
		if (LoadPrograms())
			RegisterKeys();
		m_menu = CreatePopupMenu();
		AppendMenu(m_menu, MF_STRING, TRAY_MENU_EXIT, L"Exit");

		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		//Actions for Register button.
		case REGISTER_BUTTON: 
			if (!RegisterKeys()) {
				ERROR_MESSAGE(L"Fail to register Hotkeys", GetLastError());
				return 0;
			}
			MESSAGE_BOX(L"Registered");
			return 0;
		//Actions for Checkbox.
		case SET_STARTUP:
		{
			BOOL checked = IsDlgButtonChecked(hwnd, SET_STARTUP);
			if (checked) {
				if (!SetRegistryStartProgram(TRUE)) {
					CheckDlgButton(m_hwnd, SET_STARTUP, BST_UNCHECKED);
				}
			}
			else {
				if (!SetRegistryStartProgram(FALSE)) {
					CheckDlgButton(m_hwnd, SET_STARTUP, BST_CHECKED);
				}
			}
			return 0;
		}

		}
		return 0;

	//The programs is minimized when it is closed
	case WM_CLOSE:
		Minimize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//When the program is forced to close.
	case WM_QUERYENDSESSION:
		CleanUp();
		return FALSE;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hwnd, &ps);
		return 0;
	}
	
	/*case WM_SYSCOMMAND:
	{
	switch (wParam & 0xfff0)
	{
	case SC_MINIMIZE:
	case SC_CLOSE:
	Minimize();
	return 0;
	}
	return 0;
	}*/

	//Paint key labels
	case WM_CTLCOLORSTATIC:
	{
		/*for (auto x : numPad) {
			HWND l = (HWND)lParam;
			if ((HWND)lParam == x) {*/
				HDC hdcStatic = (HDC)wParam;
				SetBkColor(hdcStatic, RGB(255, 255, 255));

				return (LRESULT)GetStockObject(NULL_BRUSH);
		/*	}
		}*/
		return 0;
		
	}

	case WM_HOTKEY:
	{	
		WORD keyPressed = LOWORD(wParam);

		if (pathMap.find(keyPressed) != pathMap.end()) {
			WCHAR *temp = pathMap[keyPressed];
			STRING s(temp);
			//If there is an open process that have the same path with
			//the path fired, switch to the process.
			//If there are more than one, switch them around instead of creating new window.
			if (!EnumOpenWindows::GetOpenProcess(openList[s], s)) {
				SwitchToThisWindow(openList[s].front(), TRUE);
			}
			else
				ShellExecute(NULL, NULL, temp, NULL, NULL, SW_SHOW);
		}
		return 0;
	}

	case WM_KEYDOWN:
	{
		WORD keyPressed = LOWORD(wParam);
		//The program is minimized when it is enabled 
		//and Escape key is pressed.
		if (IsWindowEnabled(m_hwnd)) {
			if (keyPressed == VK_ESCAPE) {

#ifdef EXIT_WITH_ESC
				//This is only for debugging purpose
				PostQuitMessage(0);
#else
				Minimize();
#endif
			}
		}
		return 0;
	}

	//Message associated with the icon in the notification area.
	case WM_TRAY:
	{
		switch (wParam)
		{
		case TRAY_ID:

			break;
		}
		if (lParam == WM_LBUTTONUP)
		{
			Restore();
			return 0;
		}
		else if (lParam == WM_RBUTTONDOWN)
		{
			POINT point;
			GetCursorPos(&point);
			SetForegroundWindow(hwnd);
			UINT clicked = TrackPopupMenu(
				m_menu,
				TPM_RETURNCMD | TPM_NONOTIFY,
				point.x,
				point.y,
				0,
				hwnd,
				NULL
			);

			if (clicked == TRAY_MENU_EXIT)
			{
				PostQuitMessage(0);
			}
			return 0;
		}
		return 0;
	}
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}