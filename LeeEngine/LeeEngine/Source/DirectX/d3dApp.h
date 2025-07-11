﻿// d3dApp.h by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드

#pragma once

#include "CoreMinimal.h"
#include "GameTimer.h"
#include "DebuggingSRV.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE GetAppInstance() const;
	HWND      GetMainWnd() const;
	float     GetWindowAspectRatio() const;

	int Run();

	// 각 프로그램에 필요한 기능들 오버라이드
	virtual bool Init();
	//bool InitImGui();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	//virtual void DrawImGui();
	virtual void DrawScene() = 0;

#ifdef WITH_EDITOR
	void DrawTitleBar();
#endif;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void TestSetWindowBarName(std::wostringstream& Test) const
	{
		SetWindowText(m_hMainWnd, Test.str().c_str());
	}

	float CurrentFrame;

protected:
	bool InitMainWindow();

	void CalculateFrameStats(); // TimeManager

	// 윈도우 관련
	HINSTANCE m_hAppInstance;
	HWND      m_hMainWnd;
	bool      m_AppPaused;
	bool      m_Minimized;
	bool      m_Maximized;
	bool      m_Resizing;

	GameTimer m_Timer;

	// 시작 값을 위해 상속받은 클래스 시작 시 값 설정 필요
	std::wstring m_MainWndTitle;
	int          m_ClientWidth;
	int          m_ClientHeight;
};
