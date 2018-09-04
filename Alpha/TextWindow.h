#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#include "BaseWindow.h"

class TextWindow : public BaseWindow<TextWindow>
{
public:
	TextWindow();
	TextWindow(const int index, 
		PCWSTR lpWindowName,
		DWORD dwStyle,
		const int x, const int y, 
		const int width, const int height, 
		HWND parentHwnd);

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





};
