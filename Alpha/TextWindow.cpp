#include "TextWindow.h"
#include <string>


TextWindow::TextWindow()
{
}

TextWindow::TextWindow(const int index, PCWSTR lpWindowName, DWORD dwStyle, const int x, const int y, const int width, const int height, HWND parentHwnd)
{
	std::wstring s(L"Window + Numpad ");
	s += std::to_wstring(index);
	const WCHAR *name = s.c_str();
	Create(name, dwStyle, NULL, x, y, width, height, parentHwnd);
}

BOOL TextWindow::Create(
	PCWSTR lpWindowName,
	DWORD dwStyle,
	DWORD dwExStyle,
	int x, int y,
	int nWidth, int nHeight,
	HWND hWndParent,
	HMENU hMenu)
{

	m_hwnd = CreateWindowW(
		L"static", lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, NULL, NULL, NULL);

	return (m_hwnd ? TRUE : FALSE);

}
