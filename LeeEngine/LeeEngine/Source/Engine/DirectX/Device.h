// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <d3d11.h>
#include "Engine/MyEngineUtils.h"
#include <wrl/client.h>

enum class EConstantBufferType
{
	CBT_PerFrame,
	CBT_PerObject,
	CBT_Light,
	CBT_SkeletalData,
	CBT_UserParam,
	CBT_ComputeShader,
	CBT_Count
};


enum class EInputLayoutType
{
	ILT_StaticMesh,
	ILT_SkeletalMesh,
	ILT_Count
};

enum class ERasterizerType
{
	RT_CullBack,
	RT_TwoSided,
	RT_WireFrame,
	RT_Count
};
enum class EBlendStateType
{
	BST_Default,
	BST_AlphaBlend,				// src(A) / Dest(1-A)
	BST_AlphaBlend_Coverage,		// 알파값에 따른 깊이 문제 해결
	BST_One_One,				// Src(1), Dest(1) - 검은색 색상 제거 or 색상 누적
	BST_Count
};
enum class DepthStencilStateType
{
	DSST_Less,			// 적으면 깊이 작성
	DSST_Less_Equal,	// <= 시 깊이값 작성
	DSST_Greater,		// 클시, 깊이값 작성 x // VolumeMesh Check
	DSST_NoTest_NoWrite,// Less, 깊이값 작성 x
	DSST_Count
};

// 언리얼엔진의 경우 RHI 를 통해 렌더링을 진행하지만 (// 언리얼엔진의 경우 GDynamicRHI 로 관리, GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, CreateInfo);)
// LeeEngine에서는 DirectX 만을 활용하므로 FDirectXDevice로 작명
class FDirectXDevice
{
public:
	FDirectXDevice(HWND* hWnd, int* ClientWidth, int* ClientHeight);
	~FDirectXDevice() = default;

	bool InitDirect3D();

	
	// 엔진 초기화 시 호출
	void BuildAllShaders();
	void OnWindowResize();
	void ResizeWindow();

	void SetViewPortSize(float x, float y);
	void SetDefaultViewPort();

	float GetAspectRatio() const {return static_cast<float>(*m_ClientWidth) / *m_ClientHeight; }

// ConstantBuffers
private:
	// 그래픽스 파이프라인에서 사용되는 상수버퍼
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffers[static_cast<UINT>(EConstantBufferType::CBT_Count)];
	void CreateConstantBuffers();
public:
	void MapConstantBuffer(EConstantBufferType Type, void* Data, size_t Size) const;
//===================================

// InputLayout
private:
	EInputLayoutType CurrentInputLayout = EInputLayoutType::ILT_Count;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayouts[static_cast<UINT>(EInputLayoutType::ILT_Count)];
public:
	void SetInputLayout(EInputLayoutType Type);

public:
	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() const { return m_d3dDevice; }
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext() const { return m_d3dDeviceContext; }
	const Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain() const { return m_SwapChain; }
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const { return m_RenderTargetView; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const { return m_DepthStencilView; }
	const D3D11_VIEWPORT* GetScreenViewport() const { return &m_ScreenViewport; }
	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() const {return m_SamplerState;}

	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetLightConstantBuffer() const {return m_LightConstantBuffer;}
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetSkeletalMeshConstantBuffer() const {return m_SkeletalMeshConstantBuffer;}


	UINT CurrentVertexShaderID = -1;
	void SetVertexShader(class FVertexShader* InVertexShader);
	UINT CurrentPixelShaderID = -1;
	void SetPixelShader(class FPixelShader* InPixelShader);

#ifdef WITH_EDITOR
public:
	const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetEditorRenderTargetTexture() const {return m_EditorRenderTargetTexture;}
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetEditorRenderTargetView() const {return m_EditorRenderTargetView;}
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  GetSRVEditorRenderTarget() const {return m_SRVEditorRenderTarget; }
	void ResizeEditorRenderTarget(float NewX,float NewY);
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_EditorRenderTargetTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_EditorRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_SRVEditorRenderTarget;
#endif	


// RasterizerState / BlendState / DepthStencilState
public:
	ID3D11RasterizerState* GetRasterizerState(ERasterizerType RSType) const {return m_RSState[static_cast<UINT>(RSType)].Get();}
	void SetRSState(ERasterizerType InRSType);
private:
	ERasterizerType CurrentRSType = ERasterizerType::RT_Count;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_RSState[static_cast<UINT>(ERasterizerType::RT_Count)];
	Microsoft::WRL::ComPtr<ID3D11BlendState>	m_BSState[static_cast<UINT>(EBlendStateType::BST_Count)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_DSState[static_cast<UINT>(DepthStencilStateType::DSST_Count)];
	void CreateRasterizerState();
	void CreateBlendState();
	void CreateDepthStencilState();

// =================================================




protected:
private:

	void InitSamplerState();

	void BuildStaticMeshShader();
	void BuildSkeletalMeshVertexShader();
	void BuildAllComputeShader();
	
public:
protected:
private:
	Microsoft::WRL::ComPtr<ID3D11Device>				m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_d3dDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_RenderTargetView;

	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_DepthStencilView;
	D3D11_VIEWPORT										m_ScreenViewport;

	// 파이프라인
	Microsoft::WRL::ComPtr<ID3D11SamplerState>	m_SamplerState;

	// 상수버퍼
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_LightConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_SkeletalMeshConstantBuffer;

	int* m_ClientWidth;
	int* m_ClientHeight;
	UINT		m_4xMsaaQuality;
	bool m_Enable4xMsaa;
	D3D_DRIVER_TYPE m_d3dDriverType;


	HWND* m_hWnd;

};