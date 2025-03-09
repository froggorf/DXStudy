// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <d3d11.h>
#include <wrl/client.h>

// 언리얼엔진의 경우 RHI 를 통해 렌더링을 진행하지만 (// 언리얼엔진의 경우 GDynamicRHI 로 관리, GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, CreateInfo);)
// LeeEngine에서는 DirectX 만을 활용하므로 FDirectXDevice로 작명

class FDirectXDevice
{
public:
	FDirectXDevice(HWND* hWnd, int* ClientWidth, int* ClientHeight);
	~FDirectXDevice() = default;

	bool InitDirect3D();
	void OnWindowResize();

	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() const { return m_d3dDevice; }
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext() const { return m_d3dDeviceContext; }
	const Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain() const { return m_SwapChain; }
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const { return m_RenderTargetView; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const { return m_DepthStencilView; }
	const D3D11_VIEWPORT* GetScreenViewport() const { return &m_ScreenViewport; }

protected:
private:
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

	int* m_ClientWidth;
	int* m_ClientHeight;
	UINT		m_4xMsaaQuality;
	bool m_Enable4xMsaa;
	D3D_DRIVER_TYPE m_d3dDriverType;


	HWND* m_hWnd;
};