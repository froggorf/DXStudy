// d3dApp.cpp by Frank Luna (C) 2011 All Rights Reserved.
// 25.01.13 루나책 코드를 이해하며 D3DX11를 D3D11로 옮긴 코드

#include "d3dApp.h"

#include <sstream>
#include <Windows.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include "imgui_internal.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEngine.h"

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
	m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_ClientWidth = 1600;
	m_ClientHeight = 1200;
	m_Enable4xMsaa = false;
	m_hMainWnd = nullptr;
	m_AppPaused = m_Minimized = m_Maximized = m_Resizing = false;
	m_4xMsaaQuality = 0;

	m_d3dDevice = nullptr;
	m_d3dDeviceContext = nullptr;
	m_SwapChain = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;

	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));

	g_d3dApp = this;
}

D3DApp::~D3DApp()
{
	// ComPtr 사용하여 Release X
	if(m_d3dDeviceContext)
	{
		m_d3dDeviceContext.Get()->ClearState();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if(GEngine)
	{
		delete GEngine;	
	}
	

	std::cout<<"??";
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

	if(!InitDirect3D())
	{
		return false;
	}

	GEngine = new UEngine(this);
	GEngine->InitEngine();

	InitImGui();

	return true;
}

bool D3DApp::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hMainWnd);
	ImGui_ImplDX11_Init(m_d3dDevice.Get(), m_d3dDeviceContext.Get());

	return true;
}

void D3DApp::OnResize()
{
	assert(m_d3dDeviceContext);
	assert(m_d3dDevice);
	assert(m_SwapChain);

	// 이전의 뷰들과 버퍼 초기화
	m_RenderTargetView.Reset();
	m_DepthStencilView.Reset();
	m_DepthStencilBuffer.Reset();

	// RTV 재생성
	if(m_SwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0))
	{
	}
	

	ComPtr<ID3D11Texture2D> backBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), 0, m_RenderTargetView.GetAddressOf());

	// backBuffer 는 ComPtr로 관리되므로 제거 x

	// 뎁스 스텐실 버퍼, 뷰 생성
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width		= m_ClientWidth;
	depthStencilDesc.Height		= m_ClientHeight;
	depthStencilDesc.MipLevels	= 1;
	depthStencilDesc.ArraySize	= 1;
	depthStencilDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 멀티 샘플링
	if(m_Enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count	= 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage			= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags		= 0;

	m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, m_DepthStencilBuffer.GetAddressOf());
	m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), 0, m_DepthStencilView.GetAddressOf());

	// 파이프라인에 바인딩
	m_d3dDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf() , m_DepthStencilView.Get());

	// 뷰포트 설정
	m_ScreenViewport.TopLeftX	= 0;
	m_ScreenViewport.TopLeftY	= 0;
	m_ScreenViewport.Width		= m_ClientWidth;
	m_ScreenViewport.Height		= m_ClientHeight;
	m_ScreenViewport.MinDepth	= 0.0f;
	m_ScreenViewport.MaxDepth	= 1.0f;

	m_d3dDeviceContext->RSSetViewports(1, &m_ScreenViewport);
	
}

void D3DApp::DrawImGui()
{
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
		if( m_d3dDevice )
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

bool D3DApp::InitDirect3D()
{
	// 디바이스, 디바이스 컨텍스트 생성
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(nullptr /*default*/, m_d3dDriverType, nullptr, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &featureLevel, m_d3dDeviceContext.GetAddressOf());
	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 4x Msaa quality를 지원하는지 확인
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality>0);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.Width  = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4xMsaa
	if(m_Enable4xMsaa)
	{
		sd.SampleDesc.Count		= 4;
		sd.SampleDesc.Quality	= m_4xMsaaQuality -1;
	}
	else
	{
		sd.SampleDesc.Count		= 1;
		sd.SampleDesc.Quality	= 0;
	}
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 2;
	sd.OutputWindow = m_hMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	// swap chain을 만들기 위해 IDXGIFactory를 디바이스로부터 생성
	IDXGIDevice* dxgiDevice = nullptr;
	if(FAILED(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		MessageBox(0, L"m_d3dDevice->QueryInterface Failed.", 0, 0);
		return false;
	}
	IDXGIAdapter* dxgiAdapter = nullptr;
	if(FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		MessageBox(0, L"GetParent Failed.", 0, 0);
		return false;
	}
	IDXGIFactory* dxgiFactory = nullptr;
	if(FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		MessageBox(0, L"dxgiAdapter->GetParent Failed.", 0, 0);
		return false;
	}
	if(FAILED(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_SwapChain.GetAddressOf())))
	{
		MessageBox(0, L"dxgiFactory->CreateSwapChain Failed.", 0, 0);
		return false;
	}
	dxgiDevice->Release();	dxgiAdapter->Release();	dxgiFactory->Release();

	OnResize();



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


