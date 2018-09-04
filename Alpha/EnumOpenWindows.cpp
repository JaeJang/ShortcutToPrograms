#include "EnumOpenWindows.h"

//PARAM:  List containing processes which have same path
//PARAM:  path to original executable file
//RETURN: true if there isn't any process that has same path for origianl executable file
BOOL EnumOpenWindows::GetOpenProcess(LIST list, STRING path)
{
	HWND foregroundWindow = GetForegroundWindow();
	//If the passed list which contains handles of same path
	//has destroied window, remove it.
	RemoveClosedWindow(list);

	//If the foreground windows module file name points to 
	//the same path to the parameter path, relocate the window handle
	//to the back of list for switching with a different window but same module.
	if (path.compare(GetProcessName(foregroundWindow)) == 0) {
		if (!list.empty())
			list.remove(foregroundWindow);
		list.push_back(foregroundWindow);
	}
	PAIR p(path, list);
	
	//Loop through open processes to find other windows
	//that have same module
	LPARAM lParam = reinterpret_cast<LPARAM>(&p);
	EnumWindows(EnumWindowsCallBack, lParam);
	return list.empty();
}

//RETURN: true for continuing iteration
BOOL EnumOpenWindows::EnumWindowsCallBack(HWND hwnd, LPARAM lParam)
{
	WCHAR tmpName[MAX_WINDOW_NAME];

	//If the window is not visible, it's not being used by user
	if (!IsWindowVisible(hwnd))
		return TRUE;
	if (!IsAltTabWindow(hwnd))
		return TRUE;
	//If the window has parent window, it's not a top window that
	//you use for executing file
	if (GetParent(hwnd))
		return TRUE;
	//If there is no title bar text for the window,
	//it's not reachable
	if (!GetWindowText(hwnd, tmpName, MAX_WINDOW_NAME))
		return TRUE;

	PAIR *p = reinterpret_cast<PAIR*>(lParam);
	
	if (p->first.compare(GetProcessName(hwnd)) == 0) {
		for (auto iter = p->second.begin(); iter != p->second.end(); ++iter) {
			//If the list already has the window, return true and continue to search
			if (*iter == hwnd)
				return TRUE;
		}
		//If the window passes all the tests, it's visible and reachable by Alt-tab sequence
		//Insert the window to the front of the list in order to switch
		//FYI, if the list is not empty, program brings the first window in the list to the screen
		p->second.push_front(hwnd);

	}

	return TRUE;
}

BOOL EnumOpenWindows::IsAltTabWindow(const HWND hwnd)
{
	HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

	HWND hwndTry;
	while ((hwndTry = GetLastActivePopup(hwndWalk)) != hwndTry) {
		if (IsWindowVisible(hwndTry)) break;
		hwndWalk = hwndTry;
	}
	return hwndWalk == hwnd;
}

//Retrieves full path for specific window
STRING EnumOpenWindows::GetProcessName(const HWND & hwnd)
{
	DWORD processId = 0;
	DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	WCHAR tmp[1024];
	GetModuleFileNameExW(hProcess, NULL, tmp, MAX_WINDOW_NAME);

	return STRING(tmp);
	
	
}

void EnumOpenWindows::RemoveClosedWindow(LIST list)
{
	for (auto iter = list.begin(); iter != list.end();) {
		if (!IsWindow(*iter))
			iter = list.erase(iter);
		else
			++iter;
	}
}
