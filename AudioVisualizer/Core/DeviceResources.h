#pragma once
#pragma comment (lib, "d3d11.lib")

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>

class DeviceResources
{
public:
	DeviceResources() = default;
	~DeviceResources() = default;

	HRESULT CreateDeviceResources(HWND hWnd);

private:

	// Direct3D Device
	Microsoft::WRL::ComPtr<ID3D11Device>			m_pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pd3dDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_pDXGISwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_pDepthStencilView;

	D3D_FEATURE_LEVEL m_featureLevel;
	D3D11_TEXTURE2D_DESC m_bbDesc;
	D3D11_VIEWPORT m_viewport;
};