// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "Device.h"
#include "Engine/UEngine.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/RenderCore/FMultiRenderTarget.h"
#include "Engine/RenderCore/RenderingThread.h"

using namespace Microsoft::WRL;

std::unique_ptr<FDirectXDevice> GDirectXDevice = nullptr;

FDirectXDevice::FDirectXDevice(HWND* hWnd, int* ClientWidth, int* ClientHeight)
{
	m_ClientWidth  = ClientWidth;
	m_ClientHeight = ClientHeight;

	m_d3dDevice          = nullptr;
	m_d3dDeviceContext   = nullptr;
	m_SwapChain          = nullptr;

	m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_Enable4xMsaa  = false;
	m_4xMsaaQuality = 0;

	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));

	m_hWnd = hWnd;

	Direct2DDevice = std::make_unique<FDirect2DDevice>();
}


bool FDirectXDevice::InitDirect3D()
{
	// 디바이스, 디바이스 컨텍스트 생성
	UINT createDeviceFlags = 0;
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 08.20 Direct2D 연동하려면 해당 플래그가 있어야한다고함
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevel{};
	HRESULT           hr = D3D11CreateDevice(nullptr /*default*/, m_d3dDriverType, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &featureLevel, m_d3dDeviceContext.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"D3D11CreateDevice Failed.", nullptr, 0);
		return false;
	}

	// 4x Msaa quality를 지원하는지 확인
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality>0);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width                   = *m_ClientWidth;
	sd.BufferDesc.Height                  = *m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator   = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4xMsaa
	if (m_Enable4xMsaa)
	{
		sd.SampleDesc.Count   = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count   = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 2;
	sd.OutputWindow = *m_hWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	// swap chain을 만들기 위해 IDXGIFactory를 디바이스로부터 생성
	IDXGIDevice* dxgiDevice = nullptr;
	if (FAILED(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		MessageBox(nullptr, L"m_d3dDevice->QueryInterface Failed.", nullptr, 0);
		return false;
	}
	IDXGIAdapter* dxgiAdapter = nullptr;
	if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		MessageBox(nullptr, L"GetParent Failed.", nullptr, 0);
		return false;
	}
	IDXGIFactory* dxgiFactory = nullptr;
	if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		MessageBox(nullptr, L"dxgiAdapter->GetParent Failed.", nullptr, 0);
		return false;
	}
	if (FAILED(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_SwapChain.GetAddressOf())))
	{
		MessageBox(nullptr, L"dxgiFactory->CreateSwapChain Failed.", nullptr, 0);
		return false;
	}
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	InitSamplerState();
	CreateRasterizerState();
	CreateBlendState();
	CreateDepthStencilState();
	CreateConstantBuffers();

	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	InitMultiRenderTarget();

	return true;
}


void FDirectXDevice::InitMultiRenderTarget()
{
	XMFLOAT2 RenderResolution = {static_cast<float>(*m_ClientWidth), static_cast<float>(*m_ClientHeight)};

	// =============
	// SwapChain MRT
	// =============
	{
		ComPtr<ID3D11Texture2D> pTex2D = nullptr;
		GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pTex2D.GetAddressOf());

		std::shared_ptr<UTexture> RTTex = std::make_shared<UTexture>();
		RTTex->Create(pTex2D);

		// 2. DepthStencilTexture 생성
		std::shared_ptr<UTexture> pDSTex = AssetManager::CreateTexture("DepthStencilTexture", (UINT)RenderResolution.x, (UINT)RenderResolution.y
																			, DXGI_FORMAT_D24_UNORM_S8_UINT
																			, D3D11_BIND_DEPTH_STENCIL
																			, D3D11_USAGE_DEFAULT);

		XMFLOAT4 ClearColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_Main] = std::make_shared<FMultiRenderTarget>();
		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_Main]->Create(&RTTex, 1, pDSTex);
		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_Main]->SetClearColor(&ClearColor, 1);

		std::shared_ptr<UTexture> RTTex_HDR = AssetManager::CreateTexture("RTT_HDR",(UINT)RenderResolution.x,(UINT)RenderResolution.y,DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		// 2. DepthStencilTexture 생성
		std::shared_ptr<UTexture> pDSTex_HDR = AssetManager::CreateTexture("DST_HDR", (UINT)RenderResolution.x, (UINT)RenderResolution.y
			, DXGI_FORMAT_D24_UNORM_S8_UINT
			, D3D11_BIND_DEPTH_STENCIL
			, D3D11_USAGE_DEFAULT);

		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_HDR] = std::make_shared<FMultiRenderTarget>();
		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_HDR]->Create(&RTTex_HDR, 1, pDSTex_HDR);
		MultiRenderTargets[(UINT)EMultiRenderTargetType::SwapChain_HDR]->SetClearColor(&ClearColor, 1);

#ifndef WITH_EDITOR
		T_PostProcess = AssetManager::CreateTexture("PP_HDR",(UINT)RenderResolution.x,(UINT)RenderResolution.y,DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
#endif
	}

#ifdef WITH_EDITOR
	// =============
	// EDITOR MRT
	// =============
	{
		CreateEditorMRT();
		
	}
#endif


	// =============
	// Deferred MRT
	// =============
	{
		XMFLOAT2 DeferredResolution = {};
#ifdef WITH_EDITOR
		DeferredResolution = {EditorViewportSize.x, EditorViewportSize.y};
#else
		DeferredResolution = {RenderResolution.x,RenderResolution.y};
		
#endif
		std::shared_ptr<UTexture> RenderTargetTextures[5] =
		{
			AssetManager::CreateTexture("ColorTargetTex", DeferredResolution.x,DeferredResolution.y
				, DXGI_FORMAT_R8G8B8A8_UNORM
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetManager::CreateTexture("NormalTargetTex", DeferredResolution.x,DeferredResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetManager::CreateTexture("PositionTargetTex", DeferredResolution.x,DeferredResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			AssetManager::CreateTexture("EmissiveTargetTex", DeferredResolution.x,DeferredResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			// r -> Metallic / g -> Specular / b -> Metallic / a -> AO
			AssetManager::CreateTexture("PBRTargetTex", DeferredResolution.x,DeferredResolution.y
					, DXGI_FORMAT_R32G32B32A32_FLOAT
					, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		XMFLOAT4 ClearColor[5] =
		{
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.0f, 0.f, 1.f),
		};

		MultiRenderTargets[(UINT)EMultiRenderTargetType::Deferred] = std::make_shared<FMultiRenderTarget>();
#ifdef WITH_EDITOR
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Deferred]->Create(RenderTargetTextures, 5, GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->GetDepthStencilTexture());
#else
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Deferred]->Create(RenderTargetTextures, 5, GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->GetDepthStencilTexture());
#endif
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Deferred]->SetClearColor(ClearColor, 5);
	}

	// =========
	// Decal MRT
	// =========
	{
		
		std::shared_ptr<UTexture> RenderTextures[2] =
		{
			UTexture::GetTextureCache("ColorTargetTex"),
			UTexture::GetTextureCache("EmissiveTargetTex"),
		};

		XMFLOAT4 ClearColor[2] =
		{
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
		};

		MultiRenderTargets[(UINT)EMultiRenderTargetType::Decal]= std::make_shared<FMultiRenderTarget>();
#ifdef WITH_EDITOR
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Decal]->Create(RenderTextures, 2, GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->GetDepthStencilTexture());
#else
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Decal]->Create(RenderTextures, 2, GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->GetDepthStencilTexture());
#endif
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Decal]->SetClearColor(ClearColor, 2);
	}


	// =========
	// Light MRT
	// =========
	{
		XMFLOAT2 Resolution = {};
#ifdef WITH_EDITOR
		Resolution = {EditorViewportSize.x, EditorViewportSize.y};
#else
		Resolution = {RenderResolution.x,RenderResolution.y};

#endif
		std::shared_ptr<UTexture> RenderTargets[2] =
		{
			AssetManager::CreateTexture("DiffuseTargetTex"
				, Resolution.x, Resolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

				AssetManager::CreateTexture("SpecularTargetTex"
					, Resolution.x, Resolution.y
					, DXGI_FORMAT_R32G32B32A32_FLOAT
					, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		XMFLOAT4 ClearColor[2] =
		{
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
			XMFLOAT4(0.f, 0.f, 0.f, 1.f),
		};

		MultiRenderTargets[static_cast<UINT>(EMultiRenderTargetType::Light)] = std::make_shared<FMultiRenderTarget>();
#ifdef WITH_EDITOR
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Light]->Create(RenderTargets, 2, GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->GetDepthStencilTexture());
#else
		MultiRenderTargets[(UINT)EMultiRenderTargetType::Light]->Create(RenderTargets, 2, GetMultiRenderTarget(EMultiRenderTargetType::SwapChain)->GetDepthStencilTexture());
#endif
		MultiRenderTargets[static_cast<UINT>(EMultiRenderTargetType::Light)]->SetClearColor(ClearColor, 2);
	}

	Direct2DDevice->Initialize(m_d3dDevice);
}


void FDirectXDevice::SetVertexShader(class FVertexShader* InVertexShader)
{
	UINT NewShaderID = InVertexShader->GetShaderID();
	if (NewShaderID != CurrentVertexShaderID)
	{
		CurrentVertexShaderID = NewShaderID;
		m_d3dDeviceContext->VSSetShader(InVertexShader->VertexShader.Get(), nullptr, 0);
		m_d3dDeviceContext->IASetInputLayout(InVertexShader->InputLayout.Get());
	}
}

void FDirectXDevice::SetPixelShader(class FPixelShader* InPixelShader)
{
	if (InPixelShader == nullptr)
	{
		CurrentPixelShaderID  = -1;
		ID3D11PixelShader* PS = nullptr;
		m_d3dDeviceContext->PSSetShader(PS, nullptr, 0);
		
		return;
	}

	UINT NewShaderID = InPixelShader->GetShaderID();
	if (NewShaderID != CurrentPixelShaderID)
	{
		CurrentPixelShaderID = NewShaderID;
		m_d3dDeviceContext->PSSetShader(InPixelShader->PixelShader.Get(), nullptr, 0);
	}
}

void FDirectXDevice::SetComputeShader(class FComputeShader* InComputeShader)
{
	UINT NewShaderID = InComputeShader->GetShaderID();
	if (InComputeShader == nullptr)
	{
		if (CurrentComputeShaderID != -1)
		{
			CurrentComputeShaderID  = -1;
			ID3D11ComputeShader* CS = nullptr;
			m_d3dDeviceContext->CSSetShader(CS, nullptr, 0);
		}
	}
	if (NewShaderID != CurrentComputeShaderID)
	{
		CurrentComputeShaderID = NewShaderID;
		m_d3dDeviceContext->CSSetShader(InComputeShader->GetComputeShader().Get(), nullptr, 0);
	}
}

void FDirectXDevice::SetGeometryShader(class FGeometryShader* InGeometryShader)
{
	if (InGeometryShader == nullptr)
	{
		if (CurrentGeometryShaderID != -1)
		{
			CurrentGeometryShaderID  = -1;
			ID3D11GeometryShader* GS = nullptr;
			m_d3dDeviceContext->GSSetShader(GS, nullptr, 0);
		}
		return;
	}

	UINT NewShaderID = InGeometryShader->GetShaderID();
	if (NewShaderID != CurrentGeometryShaderID)
	{
		CurrentGeometryShaderID = NewShaderID;
		m_d3dDeviceContext->GSSetShader(InGeometryShader->GeometryShader.Get(), nullptr, 0);
	}
}

#ifdef WITH_EDITOR
void FDirectXDevice::ResizeEditorRenderTarget(float NewX, float NewY)
{
	NewX = max(2.0f, NewX);
	NewY = max(2.0f, NewY);

	EditorViewportSize = {NewX, NewY};
	InitMultiRenderTarget();
	//CreateEditorMRT();
}
#endif

void FDirectXDevice::SetRSState(ERasterizerType InRSType)
{
	if (InRSType != CurrentRSType)
	{
		m_d3dDeviceContext->RSSetState(m_RSState[static_cast<UINT>(InRSType)].Get());
		CurrentRSType = InRSType;
	}
}

void FDirectXDevice::SetDSState(EDepthStencilStateType InDSType, UINT StencilRef)
{
	m_d3dDeviceContext->OMSetDepthStencilState(m_DSState[static_cast<UINT>(InDSType)].Get(), StencilRef);
	CurrentDSType = InDSType;
}

void FDirectXDevice::SetBSState(EBlendStateType InBSType)
{
	if (InBSType != CurrentBSType)
	{
		m_d3dDeviceContext->OMSetBlendState(m_BSState[static_cast<UINT>(InBSType)].Get(), nullptr, 0xffffffff);
		CurrentBSType = InBSType;
	}
}

const std::shared_ptr<UTexture>& FDirectXDevice::GetHDRRenderTargetTexture()
{
#ifdef WITH_EDITOR
	return GetMultiRenderTarget(EMultiRenderTargetType::Editor_HDR)->GetRenderTargetTexture(0);
#else
	return GetMultiRenderTarget(EMultiRenderTargetType::SwapChain_HDR)->GetRenderTargetTexture(0);
#endif
}

void FDirectXDevice::CreateRasterizerState()
{
	m_RSState[static_cast<UINT>(ERasterizerType::RT_CullBack)] = nullptr;

	D3D11_RASTERIZER_DESC RasterizerDesc{};

	// Cull Front
	RasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	RasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;	
	GDirectXDevice->GetDevice()->CreateRasterizerState(&RasterizerDesc, m_RSState[static_cast<UINT>(ERasterizerType::RT_CullFront)].GetAddressOf());

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
	m_BSState[static_cast<UINT>(EBlendStateType::BST_Default)] = nullptr;

	// Alphablend
	D3D11_BLEND_DESC BlendDesc = {};

	BlendDesc.RenderTarget[0].BlendEnable           = true;
	BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[static_cast<UINT>(EBlendStateType::BST_AlphaBlend)].GetAddressOf());

	// Alphablend_Coverave
	BlendDesc.AlphaToCoverageEnable = true;
	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[static_cast<UINT>(EBlendStateType::BST_AlphaBlend_Coverage)].GetAddressOf());

	// One One
	BlendDesc.AlphaToCoverageEnable = true;

	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlend    = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend   = D3D11_BLEND_ONE;

	m_d3dDevice->CreateBlendState(&BlendDesc, m_BSState[static_cast<UINT>(EBlendStateType::BST_One_One)].GetAddressOf());

	// Decal
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = true; // 각 타겟별로 블렌딩 공식을 다르게 적용한다.

	// 0 번 타겟은 AlaphaBlend
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	// 1 번 타겟 One_One 
	BlendDesc.RenderTarget[1].BlendEnable = true;
	BlendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;

	BlendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;

	BlendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	GDirectXDevice->GetDevice()->CreateBlendState(&BlendDesc, m_BSState[static_cast<UINT>(EBlendStateType::BST_Decal)].GetAddressOf());

	SetBSState(EBlendStateType::BST_AlphaBlend);
}

void FDirectXDevice::CreateDepthStencilState()
{
	// Less
	m_DSState[static_cast<UINT>(EDepthStencilStateType::DST_LESS)] = nullptr;

	// LessEqual
	D3D11_DEPTH_STENCIL_DESC Desc = {};

	Desc.DepthEnable    = true;
	Desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.StencilEnable  = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[static_cast<UINT>(EDepthStencilStateType::DST_LESS_EQUAL)]. GetAddressOf()));

	// NO_WRITE
	Desc.DepthEnable    = true;
	Desc.DepthFunc      = D3D11_COMPARISON_LESS;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Desc.StencilEnable  = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[static_cast<UINT>(EDepthStencilStateType::DST_NO_WRITE)]. GetAddressOf()));

	// NO_TEST_NO_WRITE
	Desc.DepthEnable = false;
	HR(m_d3dDevice->CreateDepthStencilState(&Desc, m_DSState[static_cast<UINT>(EDepthStencilStateType:: DST_NO_TEST_NO_WRITE)].GetAddressOf()));

	// VOLUE_CHECK
	// 뒷면을 렌더링할때 사용
	{
		// 테스트가 성공했다, 성공한 영역은 VolumeMesh 뒷면보다 앞쪽에 있다.
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_GREATER;
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 실제가 없기때문에, 자신의 깊이를 남기지 않는다.

		Desc.StencilEnable = true;	
		Desc.StencilReadMask = 0xff;
		Desc.StencilWriteMask = 0xff;

		Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 무조건 통과
		Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;// 깊이 테스트와 스텐실 테스트 모두 성공한 경우 옵션
		Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

		Desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 특정 전달값과 Stencil 값이 일치할 경우 통과
		Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;// 깊이 테스트와 스텐실 테스트 모두 성공한 경우 옵션
		Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

		GDirectXDevice->GetDevice()->CreateDepthStencilState(&Desc, m_DSState[static_cast<UINT>(EDepthStencilStateType::VOLUME_CHECK)].GetAddressOf());
	}

	// STENCIL_EQUAL
	{
		// 테스트가 성공했다, 지정된 Stencil 값을 보유한 영역만 통과
		Desc.DepthEnable = true;
		Desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // VolueMesh 는 실제가 없기때문에, 자신의 깊이를 남기지 않는다.

		Desc.StencilEnable = true;
		Desc.StencilReadMask = 0xff;
		Desc.StencilWriteMask = 0xff;
		Desc.BackFace.StencilFunc = D3D11_COMPARISON_LESS; // 특정 전달값과 Stencil 값이 일치할 경우 통과
		Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;// VolumeMesh 내부영역에 대해서 처리를 한 이후에 다시 Stencil 값을 0로 클리어
		Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

		Desc.FrontFace = Desc.BackFace;

		GDirectXDevice->GetDevice()->CreateDepthStencilState(&Desc, m_DSState[static_cast<UINT>(EDepthStencilStateType::STENCIL_EQUAL)].GetAddressOf());
	}
}

void FDirectXDevice::InitSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter   = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxLOD   = 0; // mipmap
	sampDesc.MinLOD   = 0;
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
	sampDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.MaxLOD   = 0; // mipmap
	sampDesc.MinLOD   = 0;
	m_d3dDevice->CreateSamplerState(&sampDesc, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->VSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->HSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->DSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->GSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->PSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());
	m_d3dDeviceContext->CSSetSamplers(1, 1, m_SamplerState2.GetAddressOf());

	// 큐브텍스쳐 샘플링
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;                
	sampDesc.MinLOD = 0;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_d3dDevice->CreateSamplerState(&sampDesc, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->VSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->HSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->DSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->GSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->PSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());
	m_d3dDeviceContext->CSSetSamplers(1, 1, m_CubeSampler.GetAddressOf());


}

#ifdef WITH_EDITOR
void FDirectXDevice::CreateEditorMRT()
{
	std::shared_ptr<UTexture> RTTex = AssetManager::CreateTexture("EditorRenderTexture",(UINT)EditorViewportSize.x,(UINT)EditorViewportSize.y,DXGI_FORMAT_B8G8R8A8_UNORM,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	// 2. DepthStencilTexture 생성
	std::shared_ptr<UTexture> pDSTex = AssetManager::CreateTexture("EditorDepthStencilTexture", (UINT)EditorViewportSize.x, (UINT)EditorViewportSize.y
		, DXGI_FORMAT_D24_UNORM_S8_UINT
		, D3D11_BIND_DEPTH_STENCIL
		, D3D11_USAGE_DEFAULT);

	XMFLOAT4 ClearColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_Main] = std::make_shared<FMultiRenderTarget>();
	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_Main]->Create(&RTTex, 1, pDSTex);
	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_Main]->SetClearColor(&ClearColor, 1);


	std::shared_ptr<UTexture> RTTex_HDR = AssetManager::CreateTexture("ERT_HDR",(UINT)EditorViewportSize.x,(UINT)EditorViewportSize.y,DXGI_FORMAT_R16G16B16A16_FLOAT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	// 2. DepthStencilTexture 생성
	std::shared_ptr<UTexture> pDSTex_HDR = AssetManager::CreateTexture("EDST_HDR", (UINT)EditorViewportSize.x, (UINT)EditorViewportSize.y
		, DXGI_FORMAT_D24_UNORM_S8_UINT
		, D3D11_BIND_DEPTH_STENCIL
		, D3D11_USAGE_DEFAULT);

	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_HDR] = std::make_shared<FMultiRenderTarget>();
	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_HDR]->Create(&RTTex_HDR, 1, pDSTex_HDR);
	MultiRenderTargets[(UINT)EMultiRenderTargetType::Editor_HDR]->SetClearColor(&ClearColor, 1);

	T_PostProcess = AssetManager::CreateTexture("PP_HDR",(UINT)EditorViewportSize.x,(UINT)EditorViewportSize.y,DXGI_FORMAT_R16G16B16A16_FLOAT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
}
#endif

void FDirectXDevice::OnWindowResize()
{
	if (RenderingThreadFrameCount > 0)
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& Dummy) { GDirectXDevice->ResizeWindow(); })
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

	// RTV 재생성
	if (m_SwapChain->ResizeBuffers(1, *m_ClientWidth, *m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0))
	{
	}

	InitMultiRenderTarget();

	//// 뷰포트 설정
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width    = static_cast<float>(*m_ClientWidth);
	m_ScreenViewport.Height   = static_cast<float>(*m_ClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	//m_d3dDeviceContext->RSSetViewports(1, &m_ScreenViewport);
}

void FDirectXDevice::ResetRenderTargets()
{
	// 파이프라인에 바인딩
	//m_d3dDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
}

void FDirectXDevice::SetViewPortSize(float x, float y)
{
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width    = x;
	m_ScreenViewport.Height   = y;
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
}

void FDirectXDevice::SetDefaultViewPort()
{
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width    = static_cast<float>(*m_ClientWidth);
	m_ScreenViewport.Height   = static_cast<float>(*m_ClientHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
}

XMFLOAT2 FDirectXDevice::GetCurrentResolution() const
{
#ifdef WITH_EDITOR
	return GetEditorResolution();
#else
	return GetResolution();
#endif
}

void FDirectXDevice::CreateConstantBuffers()
{
	// Constant Buffer 생성
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags         = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags    = D3D11_CPU_ACCESS_WRITE;

	// FrameConstantBuffer
	bufferDesc.ByteWidth = sizeof(FrameConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_PerFrame)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->GSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());
	m_d3dDeviceContext->CSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerFrame), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerFrame)].GetAddressOf());

	// ObjectConstantBuffer
	bufferDesc.ByteWidth = sizeof(ObjConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_PerObject)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerObject), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerObject)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_PerObject), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_PerObject)].GetAddressOf());

	// FWidgetConstantBuffer
	bufferDesc.ByteWidth = sizeof(FWidgetConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_Widget)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_Widget), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Widget)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_Widget), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Widget)].GetAddressOf());

	// SkeletalMeshBoneTransformConstantBuffer
	bufferDesc.ByteWidth = sizeof(SkeletalMeshBoneTransformConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_SkeletalData)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SkeletalData), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SkeletalData)].GetAddressOf());

	// FDebugColor
	bufferDesc.ByteWidth = sizeof(FSystemParamConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_SystemParam)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SystemParam), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SystemParam)].GetAddressOf());
	m_d3dDeviceContext->CSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SystemParam), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SystemParam)].GetAddressOf());
	m_d3dDeviceContext->GSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SystemParam), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SystemParam)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_SystemParam), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_SystemParam)].GetAddressOf());

	// LightIndex
	bufferDesc.ByteWidth = sizeof(FLightInfoConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, ConstantBuffers[static_cast<UINT>( EConstantBufferType::CBT_LightIndex)].GetAddressOf()));
	m_d3dDeviceContext->VSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_LightIndex), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_LightIndex)].GetAddressOf());
	m_d3dDeviceContext->GSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_LightIndex), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_LightIndex)].GetAddressOf());
	m_d3dDeviceContext->PSSetConstantBuffers(static_cast<UINT>(EConstantBufferType::CBT_LightIndex), 1, ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_LightIndex)].GetAddressOf());
}

void FDirectXDevice::MapConstantBuffer(EConstantBufferType Type, void* Data, size_t Size) const
{
	if (nullptr == ConstantBuffers[static_cast<UINT>(Type)])
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	HR(m_d3dDeviceContext->Map(ConstantBuffers[static_cast<UINT>(Type)].Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));
	memcpy(cbMapSub.pData, Data, Size);
	m_d3dDeviceContext->Unmap(ConstantBuffers[static_cast<UINT>(Type)].Get(), 0);
}
