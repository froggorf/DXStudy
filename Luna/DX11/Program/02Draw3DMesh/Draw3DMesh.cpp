//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include "../../Core/d3dApp.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct FrameConstantBuffer
{
	XMMATRIX View;
	XMMATRIX Projection;
};

struct ObjConstantBuffer
{
	XMMATRIX World;
};
 
class ShadowApp : public D3DApp
{
public:
	ShadowApp(HINSTANCE hInstance);
	~ShadowApp();

	// Init
	bool Init() override;
	void OnResize() override;
	void UpdateScene(float dt) override;
	void DrawScene() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
	void BuildGeometryBuffers();
	void BuildShader();



private:
	ComPtr<ID3D11Buffer>		m_BoxVertexBuffer;
	ComPtr<ID3D11Buffer>		m_BoxIndexBuffer;

	ComPtr<ID3D11VertexShader>	m_VertexShader;
	ComPtr<ID3D11PixelShader>	m_PixelShader;
	ComPtr<ID3D11InputLayout>	m_InputLayout;

	ComPtr<ID3D11Buffer>		m_FrameConstantBuffer;
	ComPtr<ID3D11Buffer>		m_ObjConstantBuffer;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;

	POINT m_LastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
#endif
	ShadowApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

ShadowApp::ShadowApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	m_MainWndTitle = L"Draw 3D Mesh";

	m_Radius = 5.0f; m_Phi = 30.0f; m_Theta = 30.0f;
	m_LastMousePos.x = 0; m_LastMousePos.y = 1;
	m_World = XMMatrixIdentity();
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();
		
}

ShadowApp::~ShadowApp()
{
	// ComPtr
}

bool ShadowApp::Init()
{
	if(!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildShader();


	return true;
}

void ShadowApp::OnResize()
{
	D3DApp::OnResize();

	// 프로젝션 매트릭스
	m_Proj = XMMatrixPerspectiveFovLH(0.5*XM_PI, GetWindowAspectRatio(), 1.0f, 1000.0f);
}

void ShadowApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = m_Radius*sinf(m_Phi)*cosf(m_Theta);
	float z = m_Radius*sinf(m_Phi)*sinf(m_Theta);
	float y = m_Radius*cosf(m_Phi);

	// 임시 테스트
	
	XMVECTOR Pos = XMVectorSet(x,y,z,1.0f);
    XMVECTOR At = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
    XMVECTOR Up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

    m_View = XMMatrixLookAtLH(Pos, At, Up);
	
}

void ShadowApp::DrawScene()
{
	const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
	m_d3dDeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_d3dDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	{
		// 셰이더 설정
		m_d3dDeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		m_d3dDeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
		// 상수버퍼 설정
		m_d3dDeviceContext->VSSetConstantBuffers(0, 1, m_FrameConstantBuffer.GetAddressOf());
		m_d3dDeviceContext->VSSetConstantBuffers(1, 1, m_ObjConstantBuffer.GetAddressOf());

		// 인풋 레이아웃
		m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());
		m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 버텍스, 인덱스 버퍼
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_d3dDeviceContext->IASetVertexBuffers(0,1, m_BoxVertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3dDeviceContext->IASetIndexBuffer(m_BoxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		

		// Frame 상수 버퍼 설정
		FrameConstantBuffer fcb;
		fcb.View = XMMatrixTranspose(m_View);
		fcb.Projection = XMMatrixTranspose(m_Proj);
		m_d3dDeviceContext->UpdateSubresource(m_FrameConstantBuffer.Get(), 0, nullptr, &fcb, 0, 0);

		// 오브젝트 Draw
		// Obj 상수 버퍼 설정
		ObjConstantBuffer ocb;
		ocb.World = XMMatrixTranspose(m_World);
		m_d3dDeviceContext->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);

		m_d3dDeviceContext->DrawIndexed(36, 0, 0);
	}

	HR(m_SwapChain->Present(0, 0));
}


void ShadowApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void ShadowApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ShadowApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Theta += dx;
		m_Phi   += dy;

		// Restrict the angle mPhi.
		m_Phi = std::clamp(m_Phi, 0.1f, XM_PI-0.1f);
		
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - m_LastMousePos.y);

		// Update the camera radius based on input.
		m_Radius += dx - dy;

		// Restrict the radius.
		m_Radius = std::clamp(m_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void ShadowApp::BuildGeometryBuffers()
{
	// 버텍스 버퍼
	Vertex vertices[] =
    {
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
    };
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(vertices);
	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = vertices;
	HR(m_d3dDevice->CreateBuffer(&vbd, &vInitData, m_BoxVertexBuffer.GetAddressOf()));
	
	// 인덱스 버퍼
	UINT indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
	};

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(indices);
	ibd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = indices;
	HR(m_d3dDevice->CreateBuffer(&ibd,&iInitData, m_BoxIndexBuffer.GetAddressOf()));
	D3D11_BUFFER_DESC test;
	m_BoxIndexBuffer.Get()->GetDesc(&test);


}

void ShadowApp::BuildShader()
{
	ComPtr<ID3DBlob> pVSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/color.fx", "VS", "vs_4_0", pVSBlob.GetAddressOf()));
	HR(m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(inputLayout);

	HR(m_d3dDevice->CreateInputLayout(inputLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_InputLayout.GetAddressOf()));
	m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());

	ComPtr<ID3DBlob> pPSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/color.fx", "PS", "ps_4_0", pPSBlob.GetAddressOf()));

	HR(m_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));


	// Constant Buffer 생성
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof( FrameConstantBuffer );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
	HR(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_FrameConstantBuffer.GetAddressOf()));

	bufferDesc.ByteWidth = sizeof(ObjConstantBuffer);
	HR(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_ObjConstantBuffer.GetAddressOf()));

}


