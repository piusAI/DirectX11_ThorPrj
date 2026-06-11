#include "TextureClass.h"

#include "DDSTextureLoader.h"

using namespace DirectX;

TextureClass::TextureClass()
{
	m_texture = 0;
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	HRESULT result;

	// Load texture data from a file by using DDS texture loader.
	result = CreateDDSTextureFromFile(device, filename, nullptr, &m_texture);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

