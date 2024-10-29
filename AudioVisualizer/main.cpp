#include <Windows.h>

// Event handling
LRESULT CALLBACK WndProc (
	HWND hWnd,       // The window's handle
	UINT message,    // The message's code
	WPARAM wParam,   // Extra message info
	LPARAM lParam    // Exra message info
)
{
	switch (message)
	{
		case WM_DESTROY: PostQuitMessage(0); break;
		default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MessageLoop()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); // Sends to WndProc
		}
		else
		{
			// Update();
			// Render();
		}
	}
}

// Replacement for main
int WINAPI wWinMain (
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow
)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);              // Size in RAM
	wcex.style = CS_HREDRAW | CS_VREDRAW;          // Window style
	wcex.lpfnWndProc = WndProc;                    // WndProc
	wcex.cbClsExtra = 0;                           // These two are for allocating extra bytes
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;                    // Handle to the instance
	wcex.hIcon = NULL;                             // Icon in the top left
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // Cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;                   // No menu
	wcex.lpszClassName = L"MainWindow";            // Class's name
	wcex.hIconSm = NULL;                           // No small icon
	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}

	// Create window
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindow(
		L"MainWindow", // Class's name
		L"Audio Visualizer", // Window Title
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Style
		CW_USEDEFAULT, // Window position X
		CW_USEDEFAULT, // Window position Y
		rc.right - rc.left, // Window Width
		rc.bottom - rc.top, // Window Height
		nullptr, // No parent window
		nullptr, // No menu
		NULL, // Handle to the instance
		nullptr // No params passed
	);

	if (!hwnd) // Check if hwnd was created
	{
		return 0;
	}

	// Display the window
	ShowWindow(hwnd, nCmdShow);

	// Main Window message loop
	MessageLoop();

	return 0;
}
