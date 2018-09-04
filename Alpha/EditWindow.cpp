#include "EditWindow.h"

EditWindow::EditWindow()
	:index(-1)
{
}

EditWindow::EditWindow(const int index, DWORD dwStyle, const int x, const int y, const int width, const int height, HWND parentHwnd)
	:index(index)
{
	Create(L"",
		dwStyle,
		NULL,
		x, y, width, height,
		parentHwnd);
}

BOOL EditWindow::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
	m_hwnd = CreateWindow(L"edit", lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, NULL, NULL, NULL);

	return (m_hwnd ? TRUE : FALSE);
}

int EditWindow::GetIndex() const
{
	return index;
}
