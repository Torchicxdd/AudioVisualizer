#include "Application.h"

Application::Application()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
}

Application::Application(const Application& copy)
{
}

Application::~Application()
{
}

bool Application::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	bool result;

	// Create and initialize the Direct3D object
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object
	m_Camera = new Camera;

	// Set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	
	// Create and initialize the model object
	m_Model = new Model;

	// Set the name of the texture file that we will be loading
	strcpy_s(textureFilename, "../Core/data/stone01.tga");

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light shader object
	m_LightShader = new LightShader;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object
	m_Light = new Light;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void Application::Shutdown()
{
	// Release the color shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	// Release the light object
	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Release the Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
	return;
}

bool Application::Frame()
{
	static float rotation = 0.0f;
	bool result;

	// Update the rotation variable each frame
	rotation -= 0.0174532925f * 0.1f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	// Render the graphics scene
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the triangle will spin
	worldMatrix = XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen
	m_Direct3D->EndScene();

	return true;
}