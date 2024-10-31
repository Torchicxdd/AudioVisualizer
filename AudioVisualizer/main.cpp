#include <Windows.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

IDXGISwapChain* swapChain;
ID3D11Device* device;
ID3D11DeviceContext* context;
D3D_FEATURE_LEVEL selectedFeatureLevel;

// Must release directx objects that start with i should be released
void CleanUp()
{
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	if (context) context->Release();
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

	CleanUp();
}

void InitD3D(HWND hwnd)
{
	// Instead of using huge D3D11CreateDeviceAndSwapChain params
	// We can use a swapChainDesc which contains everything
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	// Zero the swapChainDesc which has reasonable default values
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// If set to 0, directx will look at the size of window to choose buffer size
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;

	// No vsync if 0/1
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// Stretch back buffer if not the same size as window
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;

	// Multisampling - Only increase Count by powers of 2
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.BufferCount = 1;

	swapChainDesc.OutputWindow = hwnd;

	// Set the app to be windowed. Can change to fullscreen later using
	// IDXGISwapChain::SetFullscreenState
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
		&swapChain,
		&device,
		&selectedFeatureLevel,
		&context
	);

}

// Event handling
LRESULT CALLBACK WndProc(
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

// Replacement for main
int WINAPI wWinMain(
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

	if (hwnd == NULL)
	{
		DWORD dwError = GetLastError();
		return HRESULT_FROM_WIN32(dwError);
	}

	// Make sure our coms are set to 0
	swapChain = 0;
	device = 0;
	context = 0;

	InitD3D(hwnd);
	ShowWindow(hwnd, nCmdShow);
	MessageLoop();

	return 0;
}
