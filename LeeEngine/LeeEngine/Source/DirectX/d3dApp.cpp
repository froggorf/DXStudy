// d3dApp.cpp by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드
#include "CoreMinimal.h"
#include "d3dApp.h"
#include "Engine/RenderCore/EditorScene.h"

namespace
{
	// 특정 메시지를 클래스의 멤버함수로 전달하기 위해
	D3DApp* g_d3dApp = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 윈도우가 만들어지기 전, 메시지를 받을 수 있어 해당 방식
	return g_d3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
{
	m_hAppInstance = hInstance;
	m_MainWndTitle = L"2019180031 D3D11";
	m_ClientWidth  = 2540;
	m_ClientHeight = 1440;
	m_hMainWnd     = nullptr;
	m_AppPaused    = m_Minimized = m_Maximized = m_Resizing = false;

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
	MSG msg = {nullptr};

	m_Timer.Reset();
	constexpr int    targetFPS       = 10000;                    // 목표 FPS
	constexpr double TargetFrameTime = 1'000'000.0f / targetFPS; // 마이크로 초
	while (msg.message != WM_QUIT)
	{
		// 윈도우 메시지
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 애니메이션, 게임 업데이트
		else
		{
			auto FrameStartTime = std::chrono::high_resolution_clock::now();
			while (true)
			{
				auto CurrentTime = std::chrono::high_resolution_clock::now();
				auto ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - FrameStartTime);

				if (ElapsedTime.count() > TargetFrameTime)
					break;

				std::this_thread::yield();
			}
			m_Timer.Tick();

			if (!m_AppPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_Timer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return static_cast<int>(msg.wParam);
}

bool D3DApp::Init()
{
	if (!InitMainWindow())
	{
		return false;
	}

	GDirectXDevice = std::make_unique<FDirectXDevice>(&m_hMainWnd, &m_ClientWidth, &m_ClientHeight);
	if (!GDirectXDevice->InitDirect3D())
	{
		return false;
	}
	OnResize();

#ifdef WITH_EDITOR
	GEngine       = std::make_shared<UEditorEngine>(this); //new UEngine(this);
	GEditorEngine = std::dynamic_pointer_cast<UEditorEngine>(GEngine);
#else
	GEngine = std::make_shared<UEngine>(this);//new UEngine(this);
#endif

	GEngine->InitEngine();

	return true;
}

void D3DApp::OnResize()
{
	GDirectXDevice->OnWindowResize();
}

#ifdef WITH_EDITOR
void D3DApp::DrawTitleBar()
{
	if (GEditorEngine)
	{
		GEditorEngine->DrawEngineTitleBar();
	}
}
#endif
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef WITH_EDITOR
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;
#endif

	if (msg == WM_CLOSE)
	{
		FScene::KillRenderingThread();
		GEngine->JoinThreadsAtDestroy();
	}
	if (msg == WM_QUIT)
	{
		return 0;
	}

	switch (msg)
	{
	case WM_NCPAINT:
#ifdef WITH_EDITOR

		DrawTitleBar();
		return 0;
#endif
		break;
	case WM_NCCALCSIZE:
		{
			if (wParam == TRUE)
			{
				// NCCALCSIZE_PARAMS 구조체를 가져옴
				auto pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

				// 클라이언트 영역의 크기를 계산 (프레임 두께를 줄이거나 늘림)
				pParams->rgrc[0].left += 0;                   // 왼쪽 프레임 두께
				pParams->rgrc[0].right -= 0;                  // 오른쪽 프레임 두께
				pParams->rgrc[0].top += WindowTitleBarHeight; // 상단 프레임 두께
				pParams->rgrc[0].bottom -= 1;                 // 하단 프레임 두께

				return 0; // 기본 동작 차단
			}
		}
		break;

	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.
	case WM_ACTIVATE:

		if (LOWORD(wParam) == WA_INACTIVE)
		{
			// 비클라이언트 영역을 강제로 다시 그리기
			RedrawWindow(m_hMainWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);

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
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (GDirectXDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (m_Minimized)
				{
					m_AppPaused = false;
					m_Minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;
					OnResize();
				}
				else if (m_Resizing)
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
		m_Resizing = true;
		m_Timer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing = false;
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
		//OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
		std::cout << "??";
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	//OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
	//case WM_MOUSEMOVE:
	//OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
	case WM_KEYDOWN:
#ifndef WITH_EDITOR
		if(GEngine)
		{
			GEngine->HandleInput(msg,wParam,lParam);
		}
#endif
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
	wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"RegisterClass Failed.", nullptr, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = {0, 0, m_ClientWidth, m_ClientHeight};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	auto Style = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	//WS_OVERLAPPED | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWndTitle.c_str(), Style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInstance, 0);
	if (!m_hMainWnd)
	{
		MessageBox(nullptr, L"CreateWindow Failed.", nullptr, 0);
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

	static int   frameCnt    = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps  = static_cast<float>(frameCnt); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << L"GameThread: " << L"    " << L"FPS: " << fps << L"    " << L"Frame Time: " << mspf << L" (ms)";
		//SetWindowText(m_hMainWnd, outs.str().c_str());
		CurrentFrame = fps;
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
