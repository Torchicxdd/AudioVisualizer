#include "Texture.h"

Texture::Texture()
{
	m_targaData = nullptr;
	m_texture = nullptr;
	m_textureView = nullptr;
}

Texture::Texture(const Texture& copy)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Load the targa image data into memory
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	// Setup the description of the texture
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(result))
	{
		return false;
	}

	// Set the row pitch of the targa image data
	rowPitch = (m_width * 4) * sizeof(unsigned char);
}