// d3dApp.cpp by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드

#include "d3dApp.h"

#include <sstream>
#include <Windows.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include "imgui_internal.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEditorEngine.h"
#include "Engine/UEngine.h"
#include "Engine/DirectX/Device.h"

namespace
{
	// 특정 메시지를 클래스의 멤버함수로 전달하기 위해
	D3DApp* g_d3dApp = nullptr;
}


LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 윈도우가 만들어지기 전, 메시지를 받을 수 있어 해당 방식
	return g_d3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
{
	m_hAppInstance = hInstance;
	m_MainWndTitle = L"2019180031 D3D11";
	m_ClientWidth = 1600;
	m_ClientHeight = 1200;
	m_hMainWnd = nullptr;
	m_AppPaused = m_Minimized = m_Maximized = m_Resizing = false;


	g_d3dApp = this;
}

D3DApp::~D3DApp()
{
	// ComPtr 사용하여 Release X

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


}

HINSTANCE D3DApp::GetAppInstance() const
{
	return m_hAppInstance;
}

HWND D3DApp::GetMainWnd() const
{
	return m_hMainWnd;
}

float D3DApp::GetWindowAspectRatio() const
{
	return static_cast<float>(m_ClientWidth) / m_ClientHeight;
}

int D3DApp::Run()
{
	MSG msg = {0};


	m_Timer.Reset();

	while(msg.message != WM_QUIT)
	{
		// 윈도우 메시지
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// 애니메이션, 게임 업데이트
		else
		{
			m_Timer.Tick();

			if(!m_AppPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_Timer.DeltaTime());		
				DrawScene();
			}else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Init()
{
	if(!InitMainWindow())
	{
		return false;
	}


	GDirectXDevice = std::make_unique<FDirectXDevice>(&m_hMainWnd, &m_ClientWidth, &m_ClientHeight);
	if(!GDirectXDevice->InitDirect3D())
	{
		return false;
	}
	OnResize();


	GEngine = std::make_shared<UEditorEngine>(this);//new UEngine(this);
#ifdef MYENGINE_BUILD_DEBUG || MYENGINE_BUILD_DEVELOPMENT
	GEditorEngine = std::dynamic_pointer_cast<UEditorEngine>(GEngine);
#endif

	GEngine->InitEngine();




	return true;
}


void D3DApp::OnResize()
{
	GDirectXDevice->OnWindowResize();
	
	
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_CLOSE)
	{
		FScene::KillRenderingThread();
		GEngine->JoinThreadsAtDestroy();
	}
	if(msg==WM_QUIT)
	{
		return 0;
	}

	// ImGui 이벤트 처리
	if(ImGui::GetCurrentContext())
	{
		 ImGuiIO& io = ImGui::GetIO();
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		   return TRUE;
		}
		if(io.WantCaptureMouse || io.WantCaptureKeyboard)
		{
			return TRUE;
		}
	}
	


	switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			m_AppPaused = true;
			m_Timer.Stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer.Start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_ClientWidth  = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if( GDirectXDevice )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( m_Minimized )
				{
					m_AppPaused = false;
					m_Minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if( m_Maximized )
				{
					m_AppPaused = false;
					m_Maximized = false;
					OnResize();
				}
				else if( m_Resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing  = true;
		m_Timer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing  = false;
		m_Timer.Start();
		OnResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hAppInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWndTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInstance, 0); 
	if( !m_hMainWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}


void D3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (m_Timer.TotalTime() - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_MainWndTitle << L"    "
			 << L"FPS: " << fps << L"    " 
			 << L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hMainWnd, outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


