#include "AudioVisualizer.h"

AudioVisualizer::AudioVisualizer()
{
	m_Input = 0;
	m_Application = 0;
}

AudioVisualizer::AudioVisualizer(const AudioVisualizer& copy)
{
	// Empty copy constructor
}

AudioVisualizer::~AudioVisualizer()
{
	// Empty Deconstructor
}

bool AudioVisualizer::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api
	InitializeWindows(screenWidth, screenHeight);

	// Create and initialize the input object. THis object will be used to handle reading the keyboard input from the user
	m_Input = new Input;

	m_Input->Initialize();

	// Create and initialize the application class object. This object will handle rendering all the graphics for this application.
	m_Application = new ApplicationClass;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hWnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void AudioVisualizer::Shutdown()
{
	// Release the application class object
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// Release the input object
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window
	ShutdownWindows();

	return;
}

void AudioVisualizer::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user
	done = false;
	while (!done)
	{
		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// If windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool AudioVisualizer::Frame()
{
	bool result;

	// Check if the suer pressed the escape and wants to exit the application
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the application class object
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK AudioVisualizer::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard
		case WM_KEYDOWN:
		{
			// If a key is pressed, then send it to the input object so it can record that state
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard
		case WM_KEYUP:
		{
			// If a key is released, then send it to the input object so it can unset the state for that key
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void AudioVisualizer::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wcex;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object
	ApplicationHandle = this;

	// Get the instance of this application
	m_hInstance = GetModuleHandle(NULL);

	// Give the application a name
	m_applicationName = L"AudioVisualizer";

	// Setup the windows class with default settings
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wcex.hIconSm		= wcex.hIcon;
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= m_applicationName;
	wcex.cbSize			= sizeof(WNDCLASSEX);

	// Register the window class
	RegisterClassEx(&wcex);

	// Determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windows mode
	if (FULL_SCREEN)
	{
		// If full screen, set the screen to maximum size of the user desktop and 32bit
		memset(&dmScreenSettings, 9, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	
		// Set the position of the window to the top left corner
		posX = posY = 0;
	}
	else
	{
		// If windowed, then set it to 800x600 res
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX,
		posY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);

	// Bring the window up on the screen and set it as main focus
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// Hide the mouse cursor
	ShowCursor(false);

	return;
}

void AudioVisualizer::ShutdownWindows()
{
	// SHow the mouse cursor
	ShowCursor(true);

	// Fix the displaying settings if leaving full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	// Remove the application instance
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// Release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
