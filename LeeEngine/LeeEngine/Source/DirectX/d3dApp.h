// d3dApp.h by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드

#pragma once


#include <sstream>
#include <string>

#include "imgui.h"
#include "ImGuizmo.h"
#include "d3dUtil.h"
#include "GameTimer.h"

#include "DebuggingSRV.h"

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
	//bool InitImGui();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	//virtual void DrawImGui();
	virtual void DrawScene() = 0;

	void DrawTitleBar();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 인풋 관련 함수
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

	virtual XMMATRIX Test_DeleteLater_GetViewMatrix() const {return XMMATRIX();}
	virtual XMMATRIX Test_DeleteLater_GetProjectionMatrix() const {return XMMATRIX();}
	virtual XMFLOAT3 Test_DELETELATER_GetCameraPosition() const {return XMFLOAT3{};	}

	void TestSetWindowBarName(std::wostringstream& Test) const { SetWindowText(m_hMainWnd, Test.str().c_str());}
public:

	float CurrentFrame;
protected:
	bool InitMainWindow();

	void CalculateFrameStats();				// TimeManager

protected:
	// 윈도우 관련
	HINSTANCE	m_hAppInstance;
	HWND		m_hMainWnd;
	bool		m_AppPaused;
	bool		m_Minimized;
	bool		m_Maximized;
	bool		m_Resizing;

	GameTimer	m_Timer;

	// 시작 값을 위해 상속받은 클래스 시작 시 값 설정 필요
	std::wstring m_MainWndTitle;
	int m_ClientWidth;
	int m_ClientHeight;
	
};

