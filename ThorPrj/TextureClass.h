#pragma once


#include <d3d11.h>

class TextureClass
{

public:
	TextureClass();
	TextureClass(const TextureClass&) {}
	~TextureClass() {}

	bool Initialize(ID3D11Device*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture() { return m_texture; }

private:
	ID3D11ShaderResourceView* m_texture;
};

