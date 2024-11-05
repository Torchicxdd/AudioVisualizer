#pragma once

#include <memory>
#include <Windows.h>
#include <d3d11.h>

#include "DeviceResources.h"

class AudioVisualizer
{
public:
	AudioVisualizer();
	~AudioVisualizer();

	HRESULT CreateDesktopWindow();

	HWND GetWindowHandle() { return m_hWnd; }

	static LRESULT CALLBACK StaticWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	);

	HRESULT Run(
		std::shared_ptr<DeviceResources> deviceResources
	);

private:
	HMENU m_hMenu;
	RECT m_rc;
	HWND m_hWnd;
};

static HINSTANCE m_hInstance;
static LPCWSTR m_windowClassName;
static LPCWSTR m_windowTitle;
