#pragma once
#pragma comment (lib, "d3d11.lib")

#include <Windows.h>
#include <d3d11.h>

class AudioVisualizer
{
public:
	AudioVisualizer() = default;
	~AudioVisualizer() = default;
	void Startup();
	void Cleanup();
	void Update(float deltaT);
	void RenderScene();
	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	static IDXGISwapChain* swapChain;
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static D3D_FEATURE_LEVEL selectedFeatureLevel;
	static LPCWSTR m_windowClassName;
	static LPCWSTR m_windowTitle;
	static HINSTANCE m_hInstance;
};
