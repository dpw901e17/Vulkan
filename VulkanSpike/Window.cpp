#include "Window.h"
#include <stdexcept>

Window::Window(HINSTANCE hInstance, LPCTSTR windowName, LPCTSTR windowTitle, int nShowCmd, int width, int height, bool fullscreen): hwnd(nullptr)
{
	this->hInstance = hInstance;
	this->windowName = windowName;
	this->windowTitle = windowTitle;
	this->nShowCmd = nShowCmd;
	this->m_Width = width;
	this->m_Height = height;
	this->fullscreen = fullscreen;
}

HWND Window::GetHandle()
{
	if (!hwnd && !IntializeWindow()) {
		throw std::runtime_error("Failed to intialize Window. Please show this error message to the developers.");
	}
	return hwnd;
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, "Are you a quitter?", "QUEST: A true quitter", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

bool Window::IntializeWindow()
{
	if (fullscreen)
	{
		HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		m_Width = mi.rcMonitor.right - mi.rcMonitor.left;
		m_Height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "ERROR - could not register window class", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	//Attempt to create window handle
	hwnd = CreateWindowEx(NULL,
		windowName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_Width, m_Height,
		NULL, NULL,
		hInstance, NULL);
	//if we failed, report it.
	if (!hwnd)
	{
		MessageBox(NULL, "ERROR - could not create window", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}
	//if we are fullscreened, do it windowed fullscreen.
	if (fullscreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, 0);
	}

	//if everything went well, show the window.
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	return true;
}
