#pragma once
#ifndef _APPLICATION_H_
#define _APPLIACATION_H_

// Includes
#include "D3dClass.h"
#include "Camera.h"
#include "Model.h"
#include "ColourShader.h"

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
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	ColourShader* m_ColourShader;
};

#endif // !_APPLICATION_H_
