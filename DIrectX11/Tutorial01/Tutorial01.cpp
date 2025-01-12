//--------------------------------------------------------------------------------------
// File: Tutorial01.cpp
//
// This application demonstrates creating a Direct3D 11 device
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------


#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include <iostream>
#include <vector>
#include "DDSTextureLoader11.h"


#include "resource.h"

using namespace DirectX;
using namespace Microsoft::WRL;


struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 TexUV;
};

struct MyVertexBuffer
{
	ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};

struct FixConstantBuffer
{
    XMMATRIX mView;
    XMMATRIX mProjection;
};

struct FrameConstantBuffer
{
	XMMATRIX mWorld;
    XMFLOAT4 vMeshColor;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = nullptr;
ID3D11Device1*          g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;
ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
IDXGISwapChain*         g_pSwapChain = nullptr;
IDXGISwapChain1*        g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Texture2D*        g_pDepthStencilTexture = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
// 0109 셰이더 관련 DXGI 오브젝트 추가
ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11PixelShader*      g_pPixelShader = nullptr;
ID3D11InputLayout*      g_pVertexInputLayout = nullptr;
std::vector<MyVertexBuffer> g_vVertexBuffer;
ID3D11Buffer*           g_pConstantBuffer = nullptr;
ID3D11Buffer*           g_pFrameConstantBuffer = nullptr;
ID3D11Texture2D*        g_pTexture = nullptr;
ID3D11ShaderResourceView* g_pResourceView = nullptr;
ID3D11SamplerState*     g_pSamplerState = nullptr;
XMMATRIX                g_modelMatrix;
XMMATRIX                g_ViewMatrix;
XMMATRIX                g_ProjectionMatrix;

ComPtr<ID3D11Device>    g_testDevice;


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
HRESULT InitShader();
HRESULT InitMatrix();
HRESULT InitVertexBufferForTutorial2();
HRESULT InitVertexBufferForTutorial4();
HRESULT InitVertexBufferForTutorial6();
HRESULT InitVertexBufferForTutorial7();

HRESULT InitTexture();

void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();



//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    // 디버깅용 콘솔 띄우기
    AllocConsole();
    freopen("CONOUT$", "wb", stdout);

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // 25.01.09 셰이더 생성 코드 추가 (튜토리얼 - 2) 
    if(FAILED(InitShader()))
    {
	    CleanupDevice();
        return 0;
    }

    // 25.01.10 3차원 변환행렬 설정
    if(FAILED(InitMatrix()))
    {
	    CleanupDevice();
        return 0;
    }
    

    // 25.01.10 튜토 4 버텍스 버퍼 생성
    if(FAILED(InitVertexBufferForTutorial7()))
    {
	    CleanupDevice();
        return 0;
    }

    if(FAILED(InitTexture()))
    {
	    CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 1: Direct3D 11 Basics",
                           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                           nullptr );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// 셰이더 로드 함수
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, g_testDevice.GetAddressOf(), &g_featureLevel, &g_pImmediateContext );
        
        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);

            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );

                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;
    
    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) g_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        
        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;


    // Depth - Stencil 텍스쳐 생성
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D(&depthDesc, nullptr, &g_pDepthStencilTexture);
    if(FAILED(hr))
    {
	    return hr;
    }

    // 뎁스 스텐실 뷰 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
    depthViewDesc.Format = depthDesc.Format;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencilTexture, &depthViewDesc, &g_pDepthStencilView);
    if(FAILED(hr))
    {
	    return hr;
    }


    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    return S_OK;
}

HRESULT InitShader()
{
    HRESULT hr;

    // 25.01.09
    // 튜토리얼2 버텍스 셰이더 컴파일
    ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"Shader/Tutorial7/Tutorial07.fxh", "VS", "vs_4_0", &pVSBlob);
    if(FAILED(hr))
    {
	    MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }
    
    // 버텍스 셰이더 생성
    hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if(FAILED(hr))
    {
	    pVSBlob->Release();
        return hr;
    }

    // 인풋 레이아웃 DESC
    D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
    UINT numElements = ARRAYSIZE(layout);

    // 인풋 레이아웃 생성
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexInputLayout);
    pVSBlob->Release();
    if(FAILED(hr))
    {
	    return hr;
    }

    // 인풋 어셈블러 설정
    g_pImmediateContext->IASetInputLayout(g_pVertexInputLayout);

    // 픽셀 셰이더 컴파일
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"Shader/Tutorial7/Tutorial07.fxh", "PS", "ps_4_0", &pPSBlob);
    if(FAILED(hr))
    {
	    MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // 픽셀 셰이더 생성
    hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if(FAILED(hr))
    {
	    return hr;
    }

    return hr;
}

HRESULT InitMatrix()
{
    HRESULT hr;

	// Constant Buffer 생성
    D3D11_BUFFER_DESC cbDesc;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof( FixConstantBuffer );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    hr = g_pd3dDevice->CreateBuffer(&bufferDesc, nullptr, &g_pConstantBuffer);
    if(FAILED(hr))
    {
	    return hr;
    }


    XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -5.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
    XMVECTOR Up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);
    g_ViewMatrix = XMMatrixLookAtLH(Eye, At, Up);

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / height, 0.01f, 100.0f);
    
    
    g_modelMatrix = XMMatrixIdentity();

    bufferDesc.ByteWidth = sizeof( FrameConstantBuffer );
    hr = g_pd3dDevice->CreateBuffer(&bufferDesc, nullptr, &g_pFrameConstantBuffer);
    if(FAILED(hr))
    {
	    return hr;
    }

    return S_OK;
}

HRESULT InitVertexBufferForTutorial2()
{
    return S_OK;
 //  HRESULT hr;

 //   // 버텍스 생성 (사각형)
	//SimpleVertex vertices[] = 
 //   {
 //       XMFLOAT3(0.75f, 0.0f, 0.5f),
 //       XMFLOAT3(0.75f, -0.5f, 0.5f),
 //       XMFLOAT3(-0.0f, -0.5f, 0.5f),

 //       XMFLOAT3(0.75f, 0.0f, 0.5f),
 //       XMFLOAT3(-0.0f, -0.5f, 0.5f),
 //       XMFLOAT3(-0.0f, -0.0f, 0.5f),
	//};

 //   // 버퍼 DESC 생성
 //   D3D11_BUFFER_DESC bufferDesc = {};
 //   bufferDesc.Usage = D3D11_USAGE_DEFAULT;
 //   bufferDesc.ByteWidth = sizeof(vertices);
 //   bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 //   bufferDesc.CPUAccessFlags = 0;

 //   D3D11_SUBRESOURCE_DATA initData = {};
 //   initData.pSysMem = vertices;
 //   ID3D11Buffer* vertexBuffer;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }
 //   g_vVertexBuffer.push_back(vertexBuffer);

 //   // 버텍스 생성 (사각형)
	//SimpleVertex vertices2[] = 
 //   {
 //       XMFLOAT3(-0.5f, 0.75f, 0.5f),
 //       XMFLOAT3(-0.25f, 0.0f, 0.5f),
 //       XMFLOAT3(-0.75f, -0.0f, 0.5f),
	//};

 //   // 버퍼 DESC 생성
 //   bufferDesc.Usage = D3D11_USAGE_DEFAULT;
 //   bufferDesc.ByteWidth = sizeof(vertices2);
 //   bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 //   bufferDesc.CPUAccessFlags = 0;

 //   
 //   initData.pSysMem = vertices2;
 //   ID3D11Buffer* vertexBuffer2;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer2);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }
 //   g_vVertexBuffer.push_back(vertexBuffer2);

 //   // 프리미티브 토폴로지 - Triangle List
 //   g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

 //   return hr;
}

HRESULT InitVertexBufferForTutorial4()
{
    HRESULT hr;

 //   SimpleVertex vertices[] =
 //   {
 //       { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
 //       { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
 //   };
 //   D3D11_BUFFER_DESC bufferDesc = {};
 //   bufferDesc.Usage = D3D11_USAGE_DEFAULT;
 //   bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 //   bufferDesc.ByteWidth = sizeof(vertices);
 //   bufferDesc.CPUAccessFlags = 0;

 //   D3D11_SUBRESOURCE_DATA initData = {};
 //   initData.pSysMem = vertices;

 //   ID3D11Buffer* tempVertexBuffer;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempVertexBuffer);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }

 //   // 이전과 다르게 인덱스 버퍼를 추가 생성
 //   WORD indices[] =
 //   {
 //       3,1,0,
 //       2,1,3,

 //       0,5,4,
 //       1,5,0,

 //       3,4,7,
 //       0,4,3,

 //       1,6,5,
 //       2,6,1,

 //       2,7,6,
 //       3,7,2,

 //       6,4,5,
 //       7,4,6,
	//};
 //   bufferDesc.ByteWidth = sizeof(indices);
 //   bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
 //   initData.pSysMem = indices;

 //   ID3D11Buffer* tempIndexBuffer;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempIndexBuffer);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }
 //   // 데이터 추가
 //   g_vVertexBuffer.push_back(MyVertexBuffer{tempVertexBuffer, tempIndexBuffer});


 //   g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return hr;
}

HRESULT InitVertexBufferForTutorial6()
{
    HRESULT hr;

 //   SimpleVertex vertices[] =
 //   { { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },

 //       { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
 //       { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },

 //       { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },

 //       { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },

 //       { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },

 //       { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
 //       { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
 //   };
 //   D3D11_BUFFER_DESC bufferDesc = {};
 //   bufferDesc.Usage = D3D11_USAGE_DEFAULT;
 //   bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 //   bufferDesc.ByteWidth = sizeof(vertices);
 //   bufferDesc.CPUAccessFlags = 0;

 //   D3D11_SUBRESOURCE_DATA initData = {};
 //   initData.pSysMem = vertices;

 //   ID3D11Buffer* tempVertexBuffer;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempVertexBuffer);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }

 //   // 이전과 다르게 인덱스 버퍼를 추가 생성
 //   WORD indices[] =
 //   {
 //       3,1,0,
 //       2,1,3,

 //       6,4,5,
 //       7,4,6,

 //       11,9,8,
 //       10,9,11,

 //       14,12,13,
 //       15,12,14,

 //       19,17,16,
 //       18,17,19,

 //       22,20,21,
 //       23,20,22
	//};
 //   bufferDesc.ByteWidth = sizeof(indices);
 //   bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
 //   initData.pSysMem = indices;

 //   ID3D11Buffer* tempIndexBuffer;
 //   hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempIndexBuffer);
 //   if(FAILED(hr))
 //   {
	//    return hr;
 //   }
 //   // 데이터 추가
 //   g_vVertexBuffer.push_back(MyVertexBuffer{tempVertexBuffer, tempIndexBuffer});


 //   g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return hr;
}

HRESULT InitVertexBufferForTutorial7()
{
    HRESULT hr;

    SimpleVertex vertices[] =
{
    { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

    { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

    { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

    { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

    { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

    { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
    { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
};
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    ID3D11Buffer* tempVertexBuffer;
    hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempVertexBuffer);
    if(FAILED(hr))
    {
	    return hr;
    }

    // 이전과 다르게 인덱스 버퍼를 추가 생성
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
	};
    bufferDesc.ByteWidth = sizeof(indices);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;

    ID3D11Buffer* tempIndexBuffer;
    hr = g_pd3dDevice->CreateBuffer(&bufferDesc, &initData, &tempIndexBuffer);
    if(FAILED(hr))
    {
	    return hr;
    }
    // 데이터 추가
    g_vVertexBuffer.push_back(MyVertexBuffer{tempVertexBuffer, tempIndexBuffer});


    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return hr;
}

HRESULT InitTexture()
{
    // https://algamja1027.tistory.com/18
    HRESULT hr;
    DirectX::ScratchImage image;
    if(FAILED(LoadFromDDSFile(L"Resource/box.dds",DDS_FLAGS_NONE, nullptr, image)))
    {
	    return hr;
    }

    hr =CreateTexture(g_pd3dDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), (ID3D11Resource**)&g_pTexture);
    if(FAILED(hr))
    {
	    return hr;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC srcDesc = {};
    srcDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srcDesc.Texture2D.MostDetailedMip = 0;
    srcDesc.Texture2D.MipLevels = 1;

    D3D11_TEXTURE2D_DESC textureDesc;
    g_pTexture->GetDesc(&textureDesc);

    srcDesc.Format = textureDesc.Format;

    hr = g_pd3dDevice->CreateShaderResourceView(g_pTexture, &srcDesc, &g_pResourceView);
    if(FAILED(hr))
    {
	    return hr;
    }


    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerState);
    if(FAILED(hr))
    {
	    return hr;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
// Render the frame
//--------------------------------------------------------------------------------------
void Render()
{
    // 튜토7 - 큐브 회전 업데이트
	{
        static float t = 0.0f;
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if(timeStart == 0)
        {
	        timeStart = timeCur;
        }
        t = (timeCur - timeStart) / 1000.0f;

        // 1st Cube: Rotate around the origin
	    g_modelMatrix = XMMatrixRotationY( 1.0f );

	}

    // Just clear the backbuffer, DepthStencilView
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::DarkGray );
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw
    {
	    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
        g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
        g_pImmediateContext->VSSetConstantBuffers(1,1, &g_pFrameConstantBuffer);
        g_pImmediateContext->PSSetConstantBuffers(1,1, &g_pFrameConstantBuffer);

        g_pImmediateContext->PSSetShaderResources(0,1,&g_pResourceView);
        g_pImmediateContext->PSSetSamplers(0,1,&g_pSamplerState);

        UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
        for(auto vertexIter = g_vVertexBuffer.begin(); vertexIter < g_vVertexBuffer.begin()+1; ++vertexIter)
        {
            D3D11_BUFFER_DESC vertexBufferDesc;
            vertexIter->vertexBuffer->GetDesc(&vertexBufferDesc);
            g_pImmediateContext->IASetVertexBuffers(0,1, &(vertexIter->vertexBuffer), &stride, &offset);

            D3D11_BUFFER_DESC indexBufferDesc;
            vertexIter->indexBuffer->GetDesc(&indexBufferDesc);
            g_pImmediateContext->IASetIndexBuffer((vertexIter->indexBuffer), DXGI_FORMAT_R16_UINT, 0);

            
            // update contant buffer
            // Draw Obj 1
            FixConstantBuffer constantBuffer;
            constantBuffer.mView = XMMatrixTranspose(g_ViewMatrix);
            constantBuffer.mProjection = XMMatrixTranspose(g_ProjectionMatrix);
            g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);

            FrameConstantBuffer frameConstantBuffer;
            frameConstantBuffer.mWorld = XMMatrixTranspose(g_modelMatrix);
            frameConstantBuffer.vMeshColor = XMFLOAT4{1.0f,1.0f,1.0f,1.0f};
            g_pImmediateContext->UpdateSubresource(g_pFrameConstantBuffer,0, nullptr, &frameConstantBuffer, 0, 0);

            g_pImmediateContext->DrawIndexed(indexBufferDesc.ByteWidth / sizeof(WORD), 0, 0);
        }
    }
    
    g_pSwapChain->Present( 0, 0 );
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain1 ) g_pSwapChain1->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext1 ) g_pImmediateContext1->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice1 ) g_pd3dDevice1->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();

    if(g_pVertexShader) g_pVertexShader->Release();
    if(g_pPixelShader) g_pPixelShader->Release();
    if(g_pVertexInputLayout) g_pVertexInputLayout->Release();

    for(auto iter = g_vVertexBuffer.begin(); iter < g_vVertexBuffer.end(); ++iter)
    {
	    if(iter->vertexBuffer) iter->vertexBuffer->Release();
        if(iter->indexBuffer) iter->indexBuffer->Release();
    }
    if(g_pConstantBuffer) g_pConstantBuffer->Release();
}
