#include "AudioVisualizer.h"

AudioVisualizer::AudioVisualizer()
{
	m_windowClassName = L"AudioVisualizer";
	m_windowTitle = L"Audio Visualizer";
	m_hInstance = nullptr;
}

AudioVisualizer::~AudioVisualizer()
{
	// Empty Deconstructor
}

HRESULT AudioVisualizer::CreateDesktopWindow()
{
	if (m_hInstance == NULL)
	{
		m_hInstance = (HINSTANCE)GetModuleHandle(NULL);
	}

	HICON hIcon = NULL;
	WCHAR szExePath[MAX_PATH];
		GetModuleFileName(NULL, szExePath, MAX_PATH);

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);                         // Size in RAM
	wcex.style = CS_HREDRAW | CS_VREDRAW;                     // Window style
	wcex.lpfnWndProc = AudioVisualizer::StaticWindowProc;     // WndProc
	wcex.cbClsExtra = 0;                                      // These two are for allocating extra bytes
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;                             // Handle to the instance
	wcex.hIcon = hIcon;                                       // Icon in the top left
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);            // Cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;                              // No menu
	wcex.lpszClassName = m_windowClassName,                   // Class's name
	wcex.hIconSm = NULL;                                      // No small icon

	if (!RegisterClassEx(&wcex))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}
	}

	// Window Settings
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;

	m_hMenu = NULL;

	int nDefaultWidth = 800;
	int nDefaultHeight = 600;

	SetRect(&m_rc, 0, 0, nDefaultWidth, nDefaultHeight);
	AdjustWindowRect(
		&m_rc,
		WS_OVERLAPPEDWINDOW,
		(m_hMenu != NULL) ? true : false
		);

	// Create Window
	m_hWnd = CreateWindow(
		m_windowClassName,	// Class's name
		m_windowTitle,		// Window Title
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // Style
		x, y,                                                      // Window position X and Y
		(m_rc.right - m_rc.left), (m_rc.bottom - m_rc.top),        // Window Width and Height
		0,                                                         // No parent window
		m_hMenu,                                                   // No menu
		m_hInstance,                                               // Handle to the instance
		0                                                          // No params passed
	);

	if (m_hWnd == NULL)
	{
		DWORD dwError = GetLastError();
		return HRESULT_FROM_WIN32(dwError);
	}

	return S_OK;
}

// Event handling with Windows message loop
LRESULT CALLBACK AudioVisualizer::StaticWindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		HMENU hMenu;
		hMenu = GetMenu(hWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(hWnd);
		UnregisterClass(
			m_windowClassName,
			m_hInstance
		);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HRESULT AudioVisualizer::Run(
	std::shared_ptr<DeviceResources> deviceResources,
	std::shared_ptr<Renderer> renderer
)
{
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update the scene.
			renderer->Update();

			// Render frames during idle time (when no messages are waiting).
			renderer->Render();

			// Present the frame to the screen.
			deviceResources->Present();
		}
	}
}

// Replacement for main
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow
)
{
	ShowWindow(hwnd, nCmdShow);
	MessageLoop();

	return 0;
}
