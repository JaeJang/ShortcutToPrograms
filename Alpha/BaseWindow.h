#pragma once
#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <iostream>
#include <stdio.h>


template <class DERIVED_TYPE>
class BaseWindow
{
public:
	

	BaseWindow(): m_hwnd(NULL){ }
	BaseWindow(const BaseWindow &obj) {
		m_hwnd = obj.m_hwnd;
	}

	virtual BOOL Create(PCWSTR, DWORD, DWORD, int, int, int, int, HWND, HMENU) = 0;
	DERIVED_TYPE operator=(DERIVED_TYPE & obj) {
		std::swap(this->m_hwnd, obj.m_hwnd);
	}
	

	HWND Window() const { return m_hwnd; }

protected:
	//virtual void InitNotifyIconData() {};
	//virtual PCWSTR ClassName() const = 0;
	//virtual LRESULT  HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hwnd;
};