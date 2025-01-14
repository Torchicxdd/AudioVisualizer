#pragma once
#ifndef _APPLICATION_H_
#define _APPLIACATION_H_

// Includes
#include "D3dClass.h"
#include "Camera.h"
#include "Model.h"
#include "LightShader.h"
#include "Light.h"

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
	bool Render(float);

private:
	D3DClass* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	LightShader* m_LightShader;
	Light* m_Light;
};

#endif // !_APPLICATION_H_
