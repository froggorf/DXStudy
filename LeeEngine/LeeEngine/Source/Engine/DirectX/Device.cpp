// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "Device.h"

#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"
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
	CreateRasterizerState();
	CreateBlendState();
	CreateDepthStencilState();
	CreateConstantBuffers();


	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}


void FDirectXDevice::SetVertexShader(class FVertexShader* InVertexShader)
{
	UINT NewShaderID = InVertexShader->GetShaderID();
	if(NewShaderID != CurrentVertexShaderID)
	{
		CurrentVertexShaderID = NewShaderID;
		m_d3dDeviceContext->VSSetShader(InVertexShader->VertexShader.Get(), nullptr,0);
		m_d3dDeviceContext->IASetInputLayout(InVertexShader->InputLayout.Get());
	}
}

void FDirectXDevice::SetPixelShader(class FPixelShader* InPixelShader)
{
	UINT NewShaderID = InPixelShader->GetShaderID();
	if(NewShaderID != CurrentPixelShaderID)
	{
		CurrentPixelShaderID = NewShaderID;
		m_d3dDeviceContext->PSSetShader(InPixelShader->PixelShader.Get(), nullptr,0);
	}
}

void FDirectXDevice::SetComputeShader(class FComputeShader* InComputeShader)
{
	UINT NewShaderID = InComputeShader->GetShaderID();
	if(InComputeShader == nullptr)
	{
		if(CurrentComputeShaderID != -1)
		{
			CurrentComputeShaderID = -1;
			ID3D11ComputeShader* CS = nullptr;
			m_d3dDeviceContext->CSSetShader(CS, nullptr,0);
		}
	}
	if(NewShaderID != CurrentComputeShaderID)
	{
		CurrentComputeShaderID = NewShaderID;
		m_d3dDeviceContext->CSSetShader(InComputeShader->GetComputeShader().Get(), nullptr,0);
	}
}

void FDirectXDevice::SetGeometryShader(class FGeometryShader* InGeometryShader)
{
	if(InGeometryShader == nullptr)
	{
		if(CurrentGeometryShaderID != -1)
		{
			CurrentGeometryShaderID = -1;
			ID3D11GeometryShader* GS = nullptr;
			m_d3dDeviceContext->GSSetShader(GS, nullptr,0);
		}
		return;
	}

	UINT NewShaderID = InGeometryShader->GetShaderID();
	if(NewShaderID != CurrentGeometryShaderID)
	{
		CurrentGeometryShaderID = NewShaderID;
		m_d3dDeviceContext->GSSetShader(InGeometryShader->GeometryShader.Get(), nullptr,0);
	}
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
	textureDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// 텍스처 생성
	hr = m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, m_EditorRenderTargetTexture.GetAddressOf());

	// 렌더 타겟 뷰 생성
	hr = m_d3dDevice->CreateRenderTargetView(m_EditorRenderTargetTexture.Get(), nullptr, m_EditorRenderTargetView.GetAddressOf());

	// DSV 생성
	// 뎁스 스텐실 버퍼/뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = textureDesc.Width;
	depthStencilDesc.Height = textureDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1; // 멀티샘플링 필요시 조절
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;


	// 기존 뎁스 스텐실 리소스 해제 (ComPtr 사용시 자동이지만 명확하게 하려면)
	m_EditorDepthStencilBuffer.Reset();
	m_EditorDepthStencilView.Reset();

	// 텍스처2D 생성
	hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_EditorDepthStencilBuffer.GetAddressOf());

	// 뷰 생성
	hr = m_d3dDevice->CreateDepthStencilView(m_EditorDepthStencilBuffer.Get(), nullptr, m_EditorDepthStencilView.GetAddressOf());
	
	// 셰이더 리소스 뷰 생성
	hr = m_d3dDevice->CreateShaderResourceView(m_EditorRenderTargetTexture.Get(), nullptr, m_SRVEditorRenderTarget.GetAddressOf());

}

void FDirectXDevice::SetRSState(ERasterizerType InRSType)
{
	if(InRSType != CurrentRSType)
	{
		m_d3dDeviceContext->RSSetState(m_RSState[static_cast<UINT>(InRSType)].Get());
		CurrentRSType = InRSType;
	}
}

void FDirectXDevice::SetDSState(EDepthStencilStateType InDSType)
{
	if(InDSType != CurrentDSType)
	{
		m_d3dDeviceContext->OMSetDepthStencilState(m_DSState[static_cast<UINT>(InDSType)].Get(),0);
		CurrentDSType = InDSType;
	}
}

void FDirectXDevice::SetBSState(EBlendStateType InBSType)
{
	if(InBSType != CurrentBSType)
	{
		m_d3dDeviceContext->OMSetBlendState(m_BSState[static_cast<UINT>(InBSType)].Get(), nullptr, 0xffffffff);
		CurrentBSType = InBSType;
	}
}

void FDirectXDevice::CreateRasterizerState()
{
	m_RSState[static_cast<UINT>(ERasterizerType::RT_CullBack)] = nullptr;

	D3D11_RASTERIZER_DESC RasterizerDesc{};

	// Two Sided
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	m_d3dDevice->CreateRasterizerState(&RasterizerDesc, m_RSState[static_cast<UINT>(ERasterizerType::RT_TwoSided)].GetAddressOf());

	// Wire Frame
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_d3dDevice->CreateRasterizerState(&RasterizerDesc, m_RSState[static_cast<UINT>(ERasterizerType::RT_WireFrame)].GetAddressOf());
}

void FDirectXDevice::CreateBlendState()
{
	// Default
	m_BSState[(UINT)EBlendStateType::BST_Default] = nullptr;


	// Alphablend
	D3D11_BLEND_DESC BlendDesc = {};

	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[(UINT)EBlendStateType::BST_AlphaBlend].GetAddressOf());


	// Alphablend_Coverave
	BlendDesc.AlphaToCoverageEnable = true;
	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[(UINT)EBlendStateType::BST_AlphaBlend_Coverage].GetAddressOf());

	// One One
	BlendDesc.AlphaToCoverageEnable = true;

	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[(UINT)EBlendStateType::BST_One_One].GetAddressOf());

	SetBSState(EBlendStateType::BST_AlphaBlend);
}

void FDirectXDevice::CreateDepthStencilState()
{
	// Less
	m_DSState[(UINT)EDepthStencilStateType::DST_LESS] = nullptr;

	// LessEqual
	D3D11_DEPTH_STENCIL_DESC Desc = {};

	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.StencilEnable = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[(UINT)EDepthStencilStateType::DST_LESS_EQUAL].GetAddressOf()));

	// Greater
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_GREATER;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.StencilEnable = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[(UINT)EDepthStencilStateType::DST_GREATER].GetAddressOf()));

	// NO_WRITE
	Desc.DepthEnable = true;
	Desc.DepthFunc = D3D11_COMPARISON_LESS;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Desc.StencilEnable = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[(UINT)EDepthStencilStateType::DST_NO_WRITE].GetAddressOf()));

	// NO_TEST_NO_WRITE
	Desc.DepthEnable = false;	
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[(UINT)EDepthStencilStateType::DST_NO_TEST_NO_WRITE].GetAddressOf()));

}

void FDirectXDevice::InitSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxLOD = 0; // mipmap
	sampDesc.MinLOD = 0;
	HR(GDirectXDevice->GetDevice()->CreateSamplerState(&sampDesc, m_SamplerState.GetAddressOf()));
	m_d3dDeviceContext->VSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
	m_d3dDeviceContext->HSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
	m_d3dDeviceContext->DSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
	m_d3dDeviceContext->GSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
	m_d3dDeviceContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
	m_d3dDeviceContext->CSSetSamplers(0, 1, m_SamplerState.GetAddressOf());

	// MinMagMip 필터링 셈플러
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.MaxLOD = 0; // mipmap
	sampDesc.MinLOD = 0;
	m_d3dDevice->CreateSamplerState(&sampDesc, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->VSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->HSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->DSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->GSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->PSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->CSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());

}



void FDirectXDevice::BuildAllComputeShader()
{
	std::shared_ptr<FSetColorCS> SetColorCS = std::make_shared<FSetColorCS>();
	FShader::AddShaderCache("FSetColorCS", SetColorCS);
}


void FDirectXDevice::BuildAllShaders()
{
	BuildAllComputeShader();
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
	if(RenderingThreadFrameCount < 1)
	{
		ResizeEditorRenderTarget(400.0f,300.0f);	
	}
	
#endif

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	// backBuffer 는 ComPtr로 관리되므로 제거 x

	// 뎁스 스텐실 버퍼, 뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width		= backBufferDesc.Width;
	depthStencilDesc.Height		= backBufferDesc.Height;
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

void FDirectXDevice::ResetRenderTargets()
{
	// 파이프라인에 바인딩
	m_d3dDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf() , m_DepthStencilView.Get());

}

void FDirectXDevice::SetViewPortSize(float x, float y)
{
	m_ScreenViewport.TopLeftX	= 0;
	m_ScreenViewport.TopLeftY	= 0;
	m_ScreenViewport.Width		= static_cast<float>( x);
	m_ScreenViewport.Height		=  static_cast<float>( y);
	m_ScreenViewport.MinDepth	= 0.0f;
	m_ScreenViewport.MaxDepth	= 1.0f;
}

void FDirectXDevice::SetDefaultViewPort()
{
	m_ScreenViewport.TopLeftX	= 0;
	m_ScreenViewport.TopLeftY	= 0;
	m_ScreenViewport.Width		= static_cast<float>( *m_ClientWidth);
	m_ScreenViewport.Height		=  static_cast<float>( *m_ClientHeight);
	m_ScreenViewport.MinDepth	= 0.0f;
	m_ScreenViewport.MaxDepth	= 1.0f;
}

void FDirectXDevice::CreateConstantBuffers()
{
	// Constant Buffer 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// FrameConstantBuffer
	bufferDesc.ByteWidth = sizeof( FrameConstantBuffer );
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->GSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->CSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());

	// ObjectConstantBuffer
	bufferDesc.ByteWidth = sizeof(ObjConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerObject)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerObject), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerObject)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerObject), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerObject)].GetAddressOf());

	// LightFrameConstantBuffer
	bufferDesc.ByteWidth = sizeof(LightFrameConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Light)].GetAddressOf()));
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_Light), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Light)].GetAddressOf());

	// SkeletalMeshBoneTransformConstantBuffer
	bufferDesc.ByteWidth = sizeof(SkeletalMeshBoneTransformConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SkeletalData)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SkeletalData), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SkeletalData)].GetAddressOf());
}

void FDirectXDevice::MapConstantBuffer(EConstantBufferType Type, void* Data, size_t Size) const
{
	if(nullptr == ConstantBuffers[static_cast<UINT>(Type)])
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	HR(m_d3dDeviceContext->Map(ConstantBuffers[static_cast<UINT>(Type)].Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));
	memcpy(cbMapSub.pData, Data, Size);
	m_d3dDeviceContext->Unmap(ConstantBuffers[static_cast<UINT>(Type)].Get(), 0);
}
