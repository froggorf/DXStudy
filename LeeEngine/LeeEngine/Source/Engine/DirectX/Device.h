// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <d3d11.h>
#include "Engine/MyEngineUtils.h"
#include <wrl/client.h>

#include "FDirect2DDevice.h"
#include "Engine/RenderCore/FMultiRenderTarget.h"

enum class EConstantBufferType
{
	CBT_PerFrame,
	CBT_PerObject,
	CBT_Widget,
	CBT_SkeletalData,
	CBT_UserParam,
	CBT_ComputeShader,
	CBT_SystemParam,
	CBT_LightIndex,
	CBT_Count
};

enum class ERasterizerType
{
	RT_CullFront,
	RT_CullBack,
	RT_TwoSided,
	RT_WireFrame,
	RT_Count
};

enum class EBlendStateType
{
	BST_Default,
	BST_AlphaBlend,          // src(A) / Dest(1-A)
	BST_AlphaBlend_Coverage, // 알파값에 따른 깊이 문제 해결
	BST_One_One,             // Src(1), Dest(1) - 검은색 색상 제거 or 색상 누적
	BST_Decal,
	BST_Count
};

enum class EDepthStencilStateType
{
	DST_LESS,       // Less, Depth Write
	DST_LESS_EQUAL, // LessEqual, Depth Write
	DST_NO_WRITE,   // Less, No write
	DST_NO_TEST_NO_WRITE,

	// 라이팅 등 볼륨메쉬 체크용
	VOLUME_CHECK,
	STENCIL_EQUAL,

	DST_COUNT
};

// 언리얼엔진의 경우 RHI 를 통해 렌더링을 진행하지만 (// 언리얼엔진의 경우 GDynamicRHI 로 관리, GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, CreateInfo);)
// LeeEngine에서는 DirectX 만을 활용하므로 FDirectXDevice로 작명
class FDirectXDevice
{
public:
	FDirectXDevice(HWND* hWnd, int* ClientWidth, int* ClientHeight);
	~FDirectXDevice() = default;

	bool InitDirect3D();

	void OnWindowResize();
	void ResizeWindow();
	void ResetRenderTargets();
	void SetViewPortSize(float x, float y);
	void SetDefaultViewPort();

	float GetAspectRatio() const
	{
		return static_cast<float>(*m_ClientWidth) / *m_ClientHeight;
	}

	XMFLOAT2 GetResolution() const
	{
		return XMFLOAT2{static_cast<float>(*m_ClientWidth), static_cast<float>(*m_ClientHeight)};
	}
#ifdef WITH_EDITOR
	XMFLOAT2 GetEditorResolution() const
	{
		return EditorViewportSize;
	}
#endif
	XMFLOAT2 GetCurrentResolution() const;

	// ConstantBuffers
private:
	// 그래픽스 파이프라인에서 사용되는 상수버퍼
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Count)];
	void                                 CreateConstantBuffers();

public:
	void MapConstantBuffer(EConstantBufferType Type, void* Data, size_t Size) const;
	//===================================

	// InputLayout

	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() const  { return m_d3dDevice; }
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext() const { return m_d3dDeviceContext; }
	const Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain() const { return m_SwapChain;  } 
	const D3D11_VIEWPORT* GetScreenViewport() const { return &m_ScreenViewport; } 
	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() const { return m_SamplerState; }
	// 텍스트 렌더링용
	const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& Get2DDeviceContext() const { return Direct2DDevice->Get2DDeviceContext();}
	FDirect2DDevice* Get2DDevice() const {return Direct2DDevice.get();}

	UINT CurrentVertexShaderID = -1;
	void SetVertexShader(class FVertexShader* InVertexShader);
	UINT CurrentPixelShaderID = -1;
	void SetPixelShader(class FPixelShader* InPixelShader);
	UINT CurrentComputeShaderID = -1;
	void SetComputeShader(class FComputeShader* InComputeShader);
	UINT CurrentGeometryShaderID = -1;
	void SetGeometryShader(class FGeometryShader* InGeometryShader);

#ifdef WITH_EDITOR
	void ResizeEditorRenderTarget(float NewX, float NewY);
#endif

	// RasterizerState / BlendState / DepthStencilState
public:
	ID3D11RasterizerState* GetRasterizerState(ERasterizerType RSType) const
	{
		return m_RSState[static_cast<UINT>(RSType)].Get();
	}

	void SetRSState(ERasterizerType InRSType);
	void SetDSState(EDepthStencilStateType InDSType, UINT StencilRef = 0);
	void SetBSState(EBlendStateType InBSType);
	std::shared_ptr<FMultiRenderTarget> GetMultiRenderTarget(EMultiRenderTargetType	Type) { return MultiRenderTargets[(UINT)Type]; }
	const std::shared_ptr<UTexture>& GetHDRRenderTargetTexture();
	std::shared_ptr<UTexture>& GetPostProcessTexture() {return T_PostProcess;};
private:
	ERasterizerType        CurrentRSType = ERasterizerType::RT_Count;
	EDepthStencilStateType CurrentDSType = EDepthStencilStateType::DST_COUNT;
	EBlendStateType        CurrentBSType = EBlendStateType::BST_Count;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_RSState[static_cast<UINT>(ERasterizerType::RT_Count)];
	Microsoft::WRL::ComPtr<ID3D11BlendState>        m_BSState[static_cast<UINT>(EBlendStateType::BST_Count)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DSState[static_cast<UINT>(EDepthStencilStateType::DST_COUNT)];
	void                                            CreateRasterizerState();
	void                                            CreateBlendState();
	void                                            CreateDepthStencilState();

	// =================================================

	void InitSamplerState();
#ifdef WITH_EDITOR
	XMFLOAT2 EditorViewportSize = {2,2};
	void CreateEditorMRT();
#endif
	void InitMultiRenderTarget();

	Microsoft::WRL::ComPtr<ID3D11Device>           m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_d3dDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>         m_SwapChain;
	D3D11_VIEWPORT                                 m_ScreenViewport;

	std::unique_ptr<FDirect2DDevice>			   Direct2DDevice;


	std::shared_ptr<FMultiRenderTarget>				MultiRenderTargets[static_cast<UINT>(EMultiRenderTargetType::Count)];
	std::shared_ptr<UTexture>						T_PostProcess;


	// 파이프라인
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState2;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_CubeSampler;


	int*            m_ClientWidth;
	int*            m_ClientHeight;
	UINT            m_4xMsaaQuality;
	bool            m_Enable4xMsaa;
	D3D_DRIVER_TYPE m_d3dDriverType;

	HWND* m_hWnd;
};
