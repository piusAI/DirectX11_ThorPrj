#include "GPUShaderClass.h"



bool GPUShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	error = wcscpy_s(vsFilename, 128, L"Colorvs.hlsl");
	if (error != 0)	return false;

	error = wcscpy_s(psFilename, 128, L"Colorps.hlsl");
	if(error!=0) return false;

	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)return false;
	return true;
}

bool GPUShaderClass::Render(ID3D11DeviceContext* deviceContext,
					int indexCount, int instanceCount, XMMATRIX worldMatrix, const XMMATRIX& transformMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;
	result = SetShaderParamters(deviceContext, worldMatrix, transformMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) return false;

	RenderShader(deviceContext, indexCount, instanceCount);
	return true;

}

bool GPUShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

	HRESULT result;
	ID3D10Blob* errorMessage =0;
	ID3D10Blob* vertexShaderBuffer =0;
	ID3D10Blob* pixelShaderBuffer =0;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//vs code 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing vs Shader File", MB_OK);
		}
		return false;
	}

	//ps code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing ps Shader File", MB_OK);
		}
		return false;
	}
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	
	///uv 좌표 2채널
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Instancing
	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;


	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result)) return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// shader 사용을 위한 상수 버퍼
	// SetShaderParameter로셰이더 내부 변수 접근
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;
	

	//texture sampler
	// 과제 PA3할때 이 Sampler Desc를 아예 새로 만들고 난뒤에 samplerDesc를 교체해버렸음
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//선형 보간
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;	//모든 mip 레벨 사용

	// m_sample state samplerstate정보를 담궈줌
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void GPUShaderClass::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	return;
}

void GPUShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt", std::ios::binary);
	fout.write((char*)errorMessage->GetBufferPointer(), errorMessage->GetBufferSize());

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일닫기
	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L" shader compile중 오류가 발생! shader-error.txt 파일 확인 요망!", shaderFilename, MB_OK);

	return;
}

bool GPUShaderClass::SetShaderParamters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, const XMMATRIX& TransformMatrix,
								XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//상수 버퍼를 잠가 쓰기 작업 가능 하도록!
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 ,&mappedResource);
	if (FAILED(result)) return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	

	//XMMATRIX finalWorld = worldMatrix * TransformMatrix;;

	
	// dataPtr->modelscale = scaleMatrix; //01T Cbuffer Scaling Test
	worldMatrix = worldMatrix * XMMatrixTranspose(TransformMatrix); //CPU랑 GPU랑 받아드리는 행렬이 다름 전치!
	dataPtr->world = worldMatrix;
	
	// SRTMatrix 행렬 곱

	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	//PS에 올림
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
		
}

void GPUShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int instanceCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);


	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//instance로 수정해야함
	deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
	return;
}


