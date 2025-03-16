// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "Device.h"

#include <cassert>
#include <iostream>
#include <windows.h>
#include <memory>

#include "DirectX/d3dUtil.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEngine.h"
#include "Engine/RenderCore/RenderingThread.h"

std::unique_ptr<FDirectXDevice> GDirectXDevice = nullptr;


FDirectXDevice::FDirectXDevice(HWND* hWnd, int* ClientWidth, int* ClientHeight)
{
	m_ClientWidth = ClientWidth;
	m_ClientHeight = ClientHeight;

	m_d3dDevice = nullptr;
	m_d3dDeviceContext = nullptr;
	m_SwapChain = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;

	m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_Enable4xMsaa = false;
	m_4xMsaaQuality = 0;

	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));

#ifdef WITH_EDITOR

	m_EditorRenderTargetTexture = nullptr;
	m_EditorRenderTargetView = nullptr;
	m_SRVEditorRenderTarget = nullptr;
#endif

	m_hWnd = hWnd;

}


bool FDirectXDevice::InitDirect3D()
{
	// 디바이스, 디바이스 컨텍스트 생성
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel{};
	HRESULT hr = D3D11CreateDevice(nullptr /*default*/, m_d3dDriverType, nullptr, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &featureLevel, m_d3dDeviceContext.GetAddressOf());
	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 4x Msaa quality를 지원하는지 확인
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality>0);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width  = *m_ClientWidth;
	sd.BufferDesc.Height = *m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4xMsaa
	if(m_Enable4xMsaa)
	{
		sd.SampleDesc.Count		= 4;
		sd.SampleDesc.Quality	= m_4xMsaaQuality -1;
	}
	else
	{
		sd.SampleDesc.Count		= 1;
		sd.SampleDesc.Quality	= 0;
	}
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 2;
	sd.OutputWindow = *m_hWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	// swap chain을 만들기 위해 IDXGIFactory를 디바이스로부터 생성
	IDXGIDevice* dxgiDevice = nullptr;
	if(FAILED(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		MessageBox(0, L"m_d3dDevice->QueryInterface Failed.", 0, 0);
		return false;
	}
	IDXGIAdapter* dxgiAdapter = nullptr;
	if(FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		MessageBox(0, L"GetParent Failed.", 0, 0);
		return false;
	}
	IDXGIFactory* dxgiFactory = nullptr;
	if(FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		MessageBox(0, L"dxgiAdapter->GetParent Failed.", 0, 0);
		return false;
	}
	if(FAILED(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_SwapChain.GetAddressOf())))
	{
		MessageBox(0, L"dxgiFactory->CreateSwapChain Failed.", 0, 0);
		return false;
	}
	dxgiDevice->Release();	dxgiAdapter->Release();	dxgiFactory->Release();


	InitSamplerState();
	CreateBuffers();

	return true;
}

void FDirectXDevice::ResizeEditorRenderTarget(float NewX, float NewY)
{
	NewX = max(2.0f,NewX);
	NewY = max(2.0f,NewY);
	HRESULT hr;
	// 텍스처 설명
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<int>(NewX);
	textureDesc.Height = static_cast<int>(NewY);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// 텍스처 생성
	hr = m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, m_EditorRenderTargetTexture.GetAddressOf());

	// 렌더 타겟 뷰 생성
	hr = m_d3dDevice->CreateRenderTargetView(m_EditorRenderTargetTexture.Get(), nullptr, m_EditorRenderTargetView.GetAddressOf());

	// 셰이더 리소스 뷰 생성
	hr = m_d3dDevice->CreateShaderResourceView(m_EditorRenderTargetTexture.Get(), nullptr, m_SRVEditorRenderTarget.GetAddressOf());

}

void FDirectXDevice::InitSamplerState()
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
	HR(GDirectXDevice->GetDevice()->CreateSamplerState(&sampDesc, m_SamplerState.GetAddressOf()));
}

void FDirectXDevice::BuildStaticMeshShader()
{
	Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;

	/*TODO: 02.07 추후 셰이더도 엔진 위치로 옮길 수 있도록 수정예정*/

	std::string TempDirectoryPath =  GEngine->GetDirectoryPath();
	std::wstring TempShaderPath = std::wstring(TempDirectoryPath.begin(), TempDirectoryPath.end());
	HR(CompileShaderFromFile((TempShaderPath +  L"/Shader/LightColor.hlsl").c_str(), "VS", "vs_4_0", pVSBlob.GetAddressOf()));
	HR(GDirectXDevice->GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_StaticMeshVertexShader.GetAddressOf()));

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA, 0}

	};
	UINT numElements = ARRAYSIZE(inputLayout);

	HR(GDirectXDevice->GetDevice()->CreateInputLayout(inputLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_InputLayout.GetAddressOf()));
	GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());

	ComPtr<ID3DBlob> pPSBlob = nullptr;
	HR(CompileShaderFromFile((TempShaderPath + L"/Shader/LightColor.hlsl").c_str(), "PS", "ps_4_0", pPSBlob.GetAddressOf()));

	HR(GDirectXDevice->GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_TexturePixelShader.GetAddressOf()));
}

void FDirectXDevice::CreateBuffers()
{
	// Constant Buffer 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof( FrameConstantBuffer );
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_FrameConstantBuffer.GetAddressOf()));

	bufferDesc.ByteWidth = sizeof(ObjConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_ObjConstantBuffer.GetAddressOf()));

	bufferDesc.ByteWidth = sizeof(LightFrameConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_LightConstantBuffer.GetAddressOf()))

}

void FDirectXDevice::BuildAllShaders()
{
	BuildStaticMeshShader();
}

void FDirectXDevice::OnWindowResize()
{
	if(RenderingThreadFrameCount > 0)
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& Dummy)
		{
			GDirectXDevice->ResizeWindow();
		})
	}
	else
	{
		ResizeWindow();
	}
}

void FDirectXDevice::ResizeWindow()
{
	HRESULT hr{};
	assert(m_d3dDeviceContext);
	assert(m_d3dDevice);
	assert(m_SwapChain);

	// 이전의 뷰들과 버퍼 초기화
	m_RenderTargetView.Reset();
	m_DepthStencilView.Reset();
	m_DepthStencilBuffer.Reset();

	// RTV 재생성
	if(m_SwapChain->ResizeBuffers(1, *m_ClientWidth, *m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0))
	{
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			backBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), 0, m_RenderTargetView.GetAddressOf());

#ifdef WITH_EDITOR
	ResizeEditorRenderTarget(400.0f,300.0f);
#endif


	// backBuffer 는 ComPtr로 관리되므로 제거 x

	// 뎁스 스텐실 버퍼, 뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width		= *m_ClientWidth;
	depthStencilDesc.Height		= *m_ClientHeight;
	depthStencilDesc.MipLevels	= 1;
	depthStencilDesc.ArraySize	= 1;
	depthStencilDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 멀티 샘플링
	if(m_Enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count	= 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage			= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags		= 0;

	m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, m_DepthStencilBuffer.GetAddressOf());
	m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), 0, m_DepthStencilView.GetAddressOf());

	// 파이프라인에 바인딩
	m_d3dDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf() , m_DepthStencilView.Get());

	// 뷰포트 설정
	m_ScreenViewport.TopLeftX	= 0;
	m_ScreenViewport.TopLeftY	= 0;
	m_ScreenViewport.Width		= static_cast<float>( *m_ClientWidth);
	m_ScreenViewport.Height		=  static_cast<float>( *m_ClientHeight);
	m_ScreenViewport.MinDepth	= 0.0f;
	m_ScreenViewport.MaxDepth	= 1.0f;

	

	m_d3dDeviceContext->RSSetViewports(1, &m_ScreenViewport);
}
