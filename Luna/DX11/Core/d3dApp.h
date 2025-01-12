// d3dApp.h by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드

#pragma once

#include <string>

#include "d3dUtil.h"
#include "GameTimer.h"

using namespace Microsoft::WRL;

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE	GetAppInstance() const;
	HWND		GetMainWnd() const;
	float		GetWindowAspectRatio() const;

	int Run();

	// 각 프로그램에 필요한 기능들 오버라이드
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 인풋 관련 함수
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();				// TimeManager

protected:
	// 윈도우 관련
	HINSTANCE	m_hAppInstance;
	HWND		m_hMainWnd;
	bool		m_AppPaused;
	bool		m_Minimized;
	bool		m_Maximized;
	bool		m_Resizing;
	UINT		m_4xMsaaQuality;

	GameTimer	m_Timer;

	// 다렉 관련
	ComPtr<ID3D11Device>			m_d3dDevice;
	ComPtr<ID3D11DeviceContext>		m_d3dDeviceContext;
	ComPtr<IDXGISwapChain>			m_SwapChain;
	ComPtr<ID3D11Texture2D>			m_DepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>	m_RenderTargetView;
	ComPtr<ID3D11DepthStencilView>	m_DepthStencilView;
	D3D11_VIEWPORT					m_ScreenViewport;

	// 시작 값을 위해 상속받은 클래스 시작 시 값 설정 필요
	std::wstring m_MainWndTitle;
	D3D_DRIVER_TYPE m_d3dDriverType;
	int m_ClientWidth;
	int m_ClientHeight;
	bool m_Enable4xMsaa;
	
};

