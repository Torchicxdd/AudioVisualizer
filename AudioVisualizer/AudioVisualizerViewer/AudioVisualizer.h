#pragma once

#ifndef _AUDIOVISUALIZER_H_
#define _AUDIOVISUALIZER_H_

// Pre-processing directives
#define WIN32_LEAN_AND_MEAN

// Includes
#include <windows.h>
#include "Input.h"
#include "Application.h"

class AudioVisualizer
{
public:
	AudioVisualizer();
	AudioVisualizer(const AudioVisualizer&);
	~AudioVisualizer();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	Input* m_Input;
	Application* m_Application;
};

// Function Prototypes
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Globals
static AudioVisualizer* ApplicationHandle = 0;

#endif