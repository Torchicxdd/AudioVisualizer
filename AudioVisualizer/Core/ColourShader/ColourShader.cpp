#include "ColorShader.h"

ColourShader::ColourShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
}

ColourShader::ColourShader(const ColourShader& copy)
{
}

ColourShader::~ColourShader()
{
}

bool ColourShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// Set the filename of the vertex shader and pixel shader
	error = wcscpy_s(vsFilename, 128, L"Colour.vs");
	if (error != 0)
	{
		return false;
	}

	error = wcscpy_s(psFilename, 128, L"Colour.ps");
	if (error != 0)
	{
		return false;
	}

	// Initialize the vertex shader and pixel shader
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColourShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects
	ShutdownShader();

	return;
}

bool ColourShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColourShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Initialize the pointers this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessge(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message, then it simply could not find the shader file itself
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			OutputShaderErrorMessge(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message, then it simply could not find the shader file itself
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}
}