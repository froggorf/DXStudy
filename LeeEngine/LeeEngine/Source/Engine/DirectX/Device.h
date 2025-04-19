// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <d3d11.h>
#include "Engine/MyEngineUtils.h"
#include <wrl/client.h>

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

	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() const { return m_d3dDevice; }
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext() const { return m_d3dDeviceContext; }
	const Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain() const { return m_SwapChain; }
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const { return m_RenderTargetView; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const { return m_DepthStencilView; }
	const D3D11_VIEWPORT* GetScreenViewport() const { return &m_ScreenViewport; }
	const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetStaticMeshInputLayout() const {return m_StaticMeshInputLayout;}
	const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetSkeletalMeshInputLayout() const {return m_SkeletalMeshInputLayout;}
	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() const {return m_SamplerState;}

	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetFrameConstantBuffer() const {return m_FrameConstantBuffer;}
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetObjConstantBuffer() const {return m_ObjConstantBuffer;}
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetLightConstantBuffer() const {return m_LightConstantBuffer;}
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetSkeletalMeshConstantBuffer() const {return m_SkeletalMeshConstantBuffer;}
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
	

protected:
private:

	void InitSamplerState();

	void BuildStaticMeshShader();
	void BuildSkeletalMeshVertexShader();
	void CreateBuffers();
	
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
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_StaticMeshInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>	m_SamplerState;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_SkeletalMeshInputLayout;
	// 상수버퍼
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_FrameConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_ObjConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_LightConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_SkeletalMeshConstantBuffer;

	int* m_ClientWidth;
	int* m_ClientHeight;
	UINT		m_4xMsaaQuality;
	bool m_Enable4xMsaa;
	D3D_DRIVER_TYPE m_d3dDriverType;


	HWND* m_hWnd;

};