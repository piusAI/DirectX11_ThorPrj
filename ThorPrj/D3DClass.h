#pragma once

//LINKING
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <Windows.h>


//D3D lnclude
#include <d3d11.h>
#include<DirectXMath.h>
using namespace DirectX;

#include "AlignedAllocationPolicy.h"

class D3DClass : public AlignedAllocationPolicy<16>
{

public:
	D3DClass() : m_swapChain(0), m_device(0), m_deviceContext(0), m_renderTargetView(0), m_depthStencilBuffer(0), m_depthAbledStencilState(0), m_depthDisabledStencilState(0), m_depthStencilView(0), m_rasterState(0) {};
	D3DClass(const D3DClass& other){}
	~D3DClass() {}
	
	// Shutdown, Init이 집중해야할 곳!!
	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	void BeginScene(float, float, float, float);

	void EndScene();

	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void SetBackBufferRenderTarget();
	void ResetViewport();

	void UpdateScene(double);
	
	void TurnZBufferOff();
	void TurnZBufferOn();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	D3D11_VIEWPORT m_viewport;

	XMMATRIX groundWorld, Scale, Translation;


	//depthstencil on/off
	ID3D11DepthStencilState* m_depthAbledStencilState; 
	ID3D11DepthStencilState* m_depthDisabledStencilState; 


	// Alpha Blend on/off
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	


};

