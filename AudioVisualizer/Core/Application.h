#pragma once
#ifndef _APPLICATION_H_
#define _APPLIACATION_H_

// Includes
#include <Windows.h>

// Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(int, int, HWND);
	void Shutdown();
	void Frame();

private:
	bool Render();

private:

};

#endif // !_APPLICATION_H_
