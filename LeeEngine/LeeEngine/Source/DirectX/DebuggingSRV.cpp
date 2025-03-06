
#if defined(DEBUG) || defined(_DEBUG)
#include "DebuggingSRV.h"
#include "d3dUtil.h"

bool DebuggingSRV::bIsInitialized = false;
Microsoft::WRL::ComPtr<ID3D11SamplerState> DebuggingSRV::m_SamplerState;
Microsoft::WRL::ComPtr<ID3D11VertexShader> DebuggingSRV::m_VertexShader;
Microsoft::WRL::ComPtr<ID3D11PixelShader> DebuggingSRV::m_PixelShader;
Microsoft::WRL::ComPtr<ID3D11InputLayout> DebuggingSRV::m_InputLayout;
Microsoft::WRL::ComPtr<ID3D11Buffer> DebuggingSRV::m_VertexBuffer;
Microsoft::WRL::ComPtr<ID3D11Buffer> DebuggingSRV::m_IndexBuffer;


void DebuggingSRV::InitializeDebuggingSRV(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	bIsInitialized = true;

	// Sampler State
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(device->CreateSamplerState(&sampDesc, m_SamplerState.GetAddressOf()));	
	}

	// VertexShader, Pixel Shader
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
		HR(CompileShaderFromFile(L"Shader/DebuggingTexture.hlsl", "VS", "vs_4_0", pVSBlob.ReleaseAndGetAddressOf()));
		HR(device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));

		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA, 0}

		};
		UINT numElements = ARRAYSIZE(inputLayout);

		HR(device->CreateInputLayout(inputLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_InputLayout.GetAddressOf()));

		Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob = nullptr;
		HR(CompileShaderFromFile(L"Shader/DebuggingTexture.hlsl", "PS", "ps_4_0", pPSBlob.GetAddressOf()));

		HR(device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));
	}

	// VertexBuffer, IndexBuffer
	{
		MyVertexData vertices[] =
		{
			{{-1.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f}},
			{{1.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f,0.0f}},
			{{-1.0f,-1.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,1.0f}},
			{{1.0f,-1.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f}},
		};

		UINT indices[] =
		{
			0,1,2,
			2,1,3
		};

		// VertexBuffer
		D3D11_BUFFER_DESC vbDesc{};
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = sizeof(vertices);
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices;
		HR(device->CreateBuffer(&vbDesc, &initData, m_VertexBuffer.GetAddressOf()));

		// IndexBuffer
		D3D11_BUFFER_DESC ibDesc{};
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = sizeof(indices);
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		initData.pSysMem = indices;
		HR(device->CreateBuffer(&ibDesc, &initData, m_IndexBuffer.GetAddressOf()));
	}
	
}

void DebuggingSRV::DrawDebuggingTexture(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext ,float topX, float topY,float width, float height, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{
	if(!bIsInitialized)
	{
		std::cout<< " 초기화 진행 X " << std::endl;
		return;
	}

	// previous viewport
	UINT size = 1;
	D3D11_VIEWPORT prevViewPort;
	deviceContext->RSGetViewports(&size,&prevViewPort);

	D3D11_VIEWPORT debuggingTextureScreenViewPort;
	debuggingTextureScreenViewPort.Width = width;
	debuggingTextureScreenViewPort.Height = height;
	debuggingTextureScreenViewPort.TopLeftX = topX;
	debuggingTextureScreenViewPort.TopLeftY = topY;
	debuggingTextureScreenViewPort.MinDepth = 0.0f;
	debuggingTextureScreenViewPort.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &debuggingTextureScreenViewPort);
	const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
	deviceContext->IASetInputLayout(m_InputLayout.Get());

	// 셰이더 설정
	deviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// 디버깅 텍스쳐 설정
	deviceContext->PSSetShaderResources(0,1,srv.GetAddressOf());
	deviceContext->PSSetSamplers(0,1,m_SamplerState.GetAddressOf());

	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0,1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(6,0,0);

	deviceContext->RSSetViewports(1, &prevViewPort);
}

#endif
