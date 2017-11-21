#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE hInstance, LPCTSTR windowName, LPCTSTR windowTitle, int nShowCmd, int width, int height, bool fullscreen);
	HWND GetHandle();
	int width() const { return m_Width; }
	int height() const { return m_Height; }
private:
	HINSTANCE hInstance;
	LPCTSTR windowName;
	LPCTSTR windowTitle;
	int m_Width;
	int m_Height;
	int nShowCmd;
	bool fullscreen;
	HWND hwnd;

	//static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool IntializeWindow();
};