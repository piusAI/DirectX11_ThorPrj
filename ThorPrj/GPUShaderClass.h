#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;



class GPUShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX modelscale;
	};
public:
	GPUShaderClass() :m_vertexShader(0), m_pixelShader(0), m_layout(0), m_matrixBuffer(0), m_sampleState(0){};
	GPUShaderClass(const GPUShaderClass&) {}
	~GPUShaderClass() {}

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown() { ShutdownShader(); }
	bool Render(ID3D11DeviceContext*, int, int, XMMATRIX, const XMMATRIX&, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetShaderParamters(ID3D11DeviceContext*, XMMATRIX, const XMMATRIX&, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	// XMMATRIX scaleMatrix = XMMatrixScaling(15.0f, 15.0f, 15.0f); //01T Cbuffer Scaling Test

	//텍스쳐 넣을떄 추가로넣어줌
	ID3D11SamplerState* m_sampleState;
};


