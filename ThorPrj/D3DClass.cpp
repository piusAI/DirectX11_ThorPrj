#include "D3DClass.h"
#include <cstdio>




bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    //unsigned long long stringLength;
    size_t stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;


    //_hInstance = _hinstance;    //받아주기

    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthEnabledStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

    D3D11_RASTERIZER_DESC rasterDesc;
    float fieldOfView, screenAspect;

    //depthstencilDesc 비활성화한거
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

    //blend alpha 구조체
    D3D11_BLEND_DESC blendStateDescription;


    // vsync 값에 따라서 D3D가 화면에 그릴 때 화면 주사율에 맞춰 render 할지 여부를 설정한다.
    m_vsync_enabled = vsync;
    

    // 컴퓨터마다 다른 주사율을 가지고 있기 때문에 이를 얻어와야 한다.
    // DX는 buffer flip과 blit 방식을 사용하는데 성능 차이가 있다.
    // DXGI : DX에서 그래픽카드, 모니터, 출력장치 등을 관리하는 인터페이스
    // Factory : 객체를 생성해주는 역할을 한다.

    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    // DXGI를 생성하는 factory를 만들어서 factory 포인터에 저장
    if (FAILED(result))
    {
        return false;
    }

    //어댑터, display, DXGI모드 등등 맞춤
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) return false;

    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) return false;

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) return false;
    
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) return false;
    
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) return false;

    for (int i = 0; i < numModes; i++)
    {
        // displayModeList
        if (displayModeList[i].Width == static_cast<unsigned int>(screenWidth))
        {
            if (displayModeList[i].Height == static_cast<unsigned int>(screenHeight))
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // desc, 비디오 카드 이름, 메모리 용량 가져오기
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }

    delete[] displayModeList;
    displayModeList = 0;

    adapterOutput->Release();
    adapterOutput = 0;

    adapter->Release();
    adapter = 0;


    factory->Release();
    factory = 0;

    // 시스템에서 새로고침 빈도 얻어서 dx 초기화!
    
    // 드디어 swap chain!!!

    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 1;

    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDesc.OutputWindow = hwnd;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.Windowed = true;
    /*if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
     
    }*/

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    swapChainDesc.Flags = 0;

    //Dx Level 설정!
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    //이제 드뎌 D3D device, context 생성할 수있음!!

    //Dx11 video 카드 설치 되어있지 않는 사람들에겐 유용한 설정

    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result))
    {
        return false;
    }

    if(!m_deviceContext)
    {
        OutputDebugStringA("DeviceContext is NULL \n");
        return false;
    }
    if (!hwnd)
    {
        OutputDebugStringA("HWND is NULL\n");
    }


    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }

    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result))   return false;

    backBufferPtr->Release();
    backBufferPtr = 0;

    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result))
    {
        return false;
    }

    ZeroMemory(&depthEnabledStencilDesc, sizeof(depthEnabledStencilDesc));

    depthEnabledStencilDesc.DepthEnable = true;
    depthEnabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthEnabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthEnabledStencilDesc.StencilEnable = true;
    depthEnabledStencilDesc.StencilReadMask = 0xFF;
    depthEnabledStencilDesc.StencilWriteMask = 0xFF;

    depthEnabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthEnabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthEnabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthEnabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthEnabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthEnabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthEnabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthEnabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = m_device->CreateDepthStencilState(&depthEnabledStencilDesc, &m_depthAbledStencilState);
    if (FAILED(result)) return false;


    m_deviceContext->OMSetDepthStencilState(m_depthAbledStencilState, 1);

    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;


    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result))
    {
        return false;
    }
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


    //rasterizer 생성
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);

    if (FAILED(result))
    {
        return false;
    }
    m_deviceContext->RSSetState(m_rasterState);

    m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;

    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

    m_deviceContext->RSSetViewports(1, &m_viewport);


    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    m_worldMatrix = XMMatrixIdentity();

    // Projection matrix
    // 3D->2D 렌더
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
    

    //여기서부터 depthdisable 넣어야하네

    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    //구조체 채우고
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    if (FAILED(result))
    {
        return false;
    }

    //blendstate 구조체!

    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));


    blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
    if (FAILED(result))
    {
        return false;
    }

    // Modify the description to create an alpha disabled blend state description.
    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void D3DClass::Shutdown() {
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if (m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }
    if (m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }
    if (m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }
    if (m_depthAbledStencilState)
    {
        m_depthAbledStencilState->Release();
        m_depthAbledStencilState = 0;
    }

    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }
    if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }
    if (m_device)
    {
        m_device->Release();
        m_device = 0;
    }
    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }

    if (m_depthDisabledStencilState)
    {
        m_depthDisabledStencilState->Release();
        m_depthDisabledStencilState = 0;
    }

    if (m_alphaEnableBlendingState)
    {
        m_alphaEnableBlendingState->Release();
        m_alphaEnableBlendingState = 0;
    }

    if (m_alphaDisableBlendingState)
    {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = 0;
    }

    return;

}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void D3DClass::EndScene()
{
    if (m_vsync_enabled)
    {
        // 새로고침 빈도 고정
        m_swapChain->Present(1, 0);
    }
    else
    {
        // ASAP 표시
        m_swapChain->Present(0, 0);
    }
    return;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

void D3DClass::SetBackBufferRenderTarget()
{
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DClass::ResetViewport()
{
    m_deviceContext->RSSetViewports(1, &m_viewport);
}

void D3DClass::UpdateScene(double time)
{
    groundWorld = XMMatrixIdentity();

    Scale = XMMatrixScaling(500.f, 10.0f, 500.0f);
    Translation = XMMatrixTranslation(0.0f, 10.0f, 0.0f);

    groundWorld = Scale * Translation;
}

void D3DClass::TurnZBufferOff()
{
    m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
    return;
}

void D3DClass::TurnZBufferOn()
{
    m_deviceContext->OMSetDepthStencilState(m_depthAbledStencilState, 1);
    return;
}

void D3DClass::TurnOnAlphaBlending()
{
    float blendFactor[4];


    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn on the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

    return;
}


void D3DClass::TurnOffAlphaBlending()
{
    float blendFactor[4];


    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn off the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

    return;
}
