#include "AudioVisualizer.h"

IDXGISwapChain* AudioVisualizer::swapChain = 0;
ID3D11Device* AudioVisualizer::device = 0;
ID3D11DeviceContext* AudioVisualizer::context = 0;
D3D_FEATURE_LEVEL AudioVisualizer::selectedFeatureLevel;
LPCWSTR AudioVisualizer::m_windowClassName = L"AudioVisualizer";
LPCWSTR AudioVisualizer::m_windowTitle = L"Audio Visualizer";
HINSTANCE AudioVisualizer::m_hInstance = nullptr;

void AudioVisualizer::Startup()
{
	// Startup
}

// Must release directx objects that start with i should be released
void AudioVisualizer::Cleanup()
{
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	if (context) context->Release();
}

void AudioVisualizer::Update(float deltaT)
{
	// Update loop
}

void AudioVisualizer::RenderScene()
{
	// Render scene
}

// Event handling
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

void MessageLoop()
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

			// Render frames during idle time (when no messages are waiting).

			// Present the frame to the screen.
		}
	}
}

void InitD3D(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Enables resource to be used as drawing surface
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Not using flags right now
	// Example flags: DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	// DXGI_SWAP_CHAIN_FLAG::
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&AudioVisualizer::swapChain,
		&AudioVisualizer::device,
		&AudioVisualizer::selectedFeatureLevel,
		&AudioVisualizer::context
	);

	hr = AudioVisualizer::swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**) &AudioVisualizer::m_pBackBuffer);

	hr = AudioVisualizer::device->CreateRenderTargetView(
		AudioVisualizer::m_pBackBuffer.Get(),
		nullptr,
		m_pRenderTarget.GetAddressOf()
	);

	AudioVisualizer::m_pBackBuffer->GetDesc(&m_bbDesc);
}

// Replacement for main
int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow
)
{
	if (hInstance == NULL)
	{
		hInstance = (HINSTANCE)GetModuleHandle(NULL);
	}
	AudioVisualizer::m_hInstance = hInstance;

	HICON hIcon = NULL;
	WCHAR szExePath[MAX_PATH];
		GetModuleFileName(NULL, szExePath, MAX_PATH);

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);                       // Size in RAM
	wcex.style = CS_HREDRAW | CS_VREDRAW;                   // Window style
	wcex.lpfnWndProc = AudioVisualizer::StaticWindowProc;   // WndProc
	wcex.cbClsExtra = 0;                                    // These two are for allocating extra bytes
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;                             // Handle to the instance
	wcex.hIcon = hIcon;                                     // Icon in the top left
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);          // Cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;                            // No menu
	wcex.lpszClassName = L"AudioVisualizer",                // Class's name
	wcex.hIconSm = NULL;                                    // No small icon
	if (!RegisterClassEx(&wcex))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}
	}

	// Create window
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindow(
		AudioVisualizer::m_windowClassName, // Class's name
		AudioVisualizer::m_windowTitle,     // Window Title
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Style
		CW_USEDEFAULT,      // Window position X
		CW_USEDEFAULT,      // Window position Y
		rc.right - rc.left, // Window Width
		rc.bottom - rc.top, // Window Height
		nullptr,            // No parent window
		nullptr,            // No menu
		NULL,               // Handle to the instance
		nullptr             // No params passed
	);

	if (hwnd == NULL)
	{
		DWORD dwError = GetLastError();
		return HRESULT_FROM_WIN32(dwError);
	}

	InitD3D(hwnd);
	ShowWindow(hwnd, nCmdShow);
	MessageLoop();

	return 0;
}
