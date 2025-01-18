//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include <vector>

#include "../../Core/d3dApp.h"
#include "../../Core/AssetManager.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

using namespace DirectX;

struct FrameConstantBuffer
{
	XMMATRIX View;
	XMMATRIX Projection;
};

struct ObjConstantBuffer
{
	XMMATRIX World;
	XMFLOAT3X3 InvTransposeMatrix;
};

class LightingApp : public D3DApp
{
public:
	LightingApp(HINSTANCE hInstance);
	~LightingApp();

	
	// Init
	bool Init() override;
	void InitSamplerState();
	void OnResize() override;
	void UpdateScene(float dt) override;
	void DrawImGui() override;
	void DrawScene() override;

	void LoadModels();

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:
	
	void BuildShader();

	void ChangeModelTexture(int bodyIndex, const ComPtr<ID3D11ShaderResourceView>& newSRV);

private:
	// 모델 정보
	std::vector<ComPtr<ID3D11Buffer>>				m_ModelVertexBuffer;
	std::vector<ComPtr<ID3D11Buffer>> 				m_ModelIndexBuffer;
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_ModelShaderResourceView;

	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_BodyShaderResourceView;
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_FaceShaderResourceView;

	// 파이프라인
	ComPtr<ID3D11VertexShader>	m_VertexShader;
	ComPtr<ID3D11PixelShader>	m_PixelShader;
	ComPtr<ID3D11InputLayout>	m_InputLayout;
	ComPtr<ID3D11SamplerState>	m_SamplerState;

	// 상수버퍼
	ComPtr<ID3D11Buffer>		m_FrameConstantBuffer;
	ComPtr<ID3D11Buffer>		m_ObjConstantBuffer;

	// 변환 행렬
	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;

	float m_Theta;
	float m_Phi;
	float m_Radius;
	POINT m_LastMousePos;

	float m_ModelScale = 0.02f;

	
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	AllocConsole();
	FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

	LightingApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

LightingApp::LightingApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	m_MainWndTitle = L"Texture To Model";

	m_Radius = 5.0f; m_Phi = 180.0f; m_Theta = 0.0f;
	m_LastMousePos.x = 0; m_LastMousePos.y = 1;
	m_World = XMMatrixIdentity();
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();
		
}

LightingApp::~LightingApp()
{
	// ComPtr
}

bool LightingApp::Init()
{
	if(!D3DApp::Init())
		return false;

	InitSamplerState();

	LoadModels();

	BuildShader();

	return true;
}

void LightingApp::LoadModels()
{
	AssetManager::LoadModelData("Model/chibi_cat.fbx", m_d3dDevice, m_ModelVertexBuffer, m_ModelIndexBuffer);

	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_01.png", m_d3dDevice, m_ModelShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_01.png", m_d3dDevice, m_ModelShaderResourceView);

	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_01.png", m_d3dDevice, m_BodyShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_02.png", m_d3dDevice, m_BodyShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_06.png", m_d3dDevice, m_BodyShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_01.png", m_d3dDevice, m_FaceShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_04.png", m_d3dDevice, m_FaceShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_25.png", m_d3dDevice, m_FaceShaderResourceView);
	m_ModelShaderResourceView.push_back(m_BodyShaderResourceView[2]);
	m_ModelShaderResourceView.push_back(m_FaceShaderResourceView[2]);
}


void LightingApp::InitSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(m_d3dDevice->CreateSamplerState(&sampDesc, &m_SamplerState));

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE; // 알파 커버리지 비활성화
	blendDesc.IndependentBlendEnable = FALSE; // 독립 블렌딩 비활성화
	blendDesc.RenderTarget[0].BlendEnable = TRUE; // 블렌딩 활성화
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 소스 알파값
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 대상 알파값의 반대
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 더하기 연산
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // 알파값 소스
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // 알파값 대상
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // 알파값 더하기 연산
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // RGBA 모두 활성화
	
	ID3D11BlendState* blendState = nullptr;
	HR(m_d3dDevice->CreateBlendState(&blendDesc, &blendState));
	
	
	// 파이프라인에 블렌딩 상태 설정
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // 블렌드 팩터 (기본값)
	UINT sampleMask = 0xFFFFFFFF; // 샘플 마스크 (기본값)
	m_d3dDeviceContext->OMSetBlendState(blendState, blendFactor, sampleMask);
}

void LightingApp::OnResize()
{
	D3DApp::OnResize();

	// 프로젝션 매트릭스
	m_Proj = XMMatrixPerspectiveFovLH(0.5*XM_PI, GetWindowAspectRatio(), 1.0f, 1000.0f);
}

void LightingApp::UpdateScene(float dt)
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

void LightingApp::DrawImGui()
{
	// (Your code process and dispatch Win32 messages)
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI 코드 작성
	ImGui::SetNextWindowSize(ImVec2(600,200));
    ImGui::Begin("Controller");
    ImGui::SliderFloat("Scale", &m_ModelScale, 0.01f, 1.0f); // 슬라이더 추가
	ImGui::Text("Body SRV Change");
	if(ImGui::Button("Body1", ImVec2(150,25)))
	{
		ChangeModelTexture(0, m_BodyShaderResourceView[0]);
	}
	ImGui::SameLine();
	if(ImGui::Button("Body2", ImVec2(150,25)))
	{
		ChangeModelTexture(0, m_BodyShaderResourceView[1]);
	}
	ImGui::SameLine();
	if(ImGui::Button("Body3", ImVec2(150,25)))
	{
		ChangeModelTexture(0, m_BodyShaderResourceView[2]);
	}
	ImGui::Text("Face SRV Change");
	if(ImGui::Button("Face1", ImVec2(150,25)))
	{
		ChangeModelTexture(1, m_FaceShaderResourceView[0]);
	}
	ImGui::SameLine();
	if(ImGui::Button("Face2", ImVec2(150,25)))
	{
		ChangeModelTexture(1, m_FaceShaderResourceView[1]);
	}
	ImGui::SameLine();
	if(ImGui::Button("Face3", ImVec2(150,25)))
	{
		ChangeModelTexture(1, m_FaceShaderResourceView[2]);
	}
    ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void LightingApp::DrawScene()
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

		// Frame 상수 버퍼 설정
		FrameConstantBuffer fcb;
		fcb.View = XMMatrixTranspose(m_View);
		fcb.Projection = XMMatrixTranspose(m_Proj);
		m_d3dDeviceContext->UpdateSubresource(m_FrameConstantBuffer.Get(), 0, nullptr, &fcb, 0, 0);
		
		// 오브젝트 Draw
		// Obj 상수 버퍼 설정
		ObjConstantBuffer ocb;
		XMMATRIX world = m_World * XMMatrixScaling(m_ModelScale,m_ModelScale,m_ModelScale);
		world *= XMMatrixTranslation(0.0f, -2.5f, 0.0f);
		ocb.World = XMMatrixTranspose(world);
		

		// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
		XMMATRIX invMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
		XMStoreFloat3x3(&ocb.InvTransposeMatrix, invMatrix);

		m_d3dDeviceContext->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);

		// Sampler State 설정
		m_d3dDeviceContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());

		// 버텍스 버퍼에 맞춰 오브젝트 드로우
		for(int vertexCount = 0; vertexCount < m_ModelVertexBuffer.size(); ++vertexCount)
		{
			// SRV 설정(텍스쳐)
			m_d3dDeviceContext->PSSetShaderResources(0,1, m_ModelShaderResourceView[vertexCount].GetAddressOf());

			UINT stride = sizeof(MyVertexData);
			UINT offset = 0;
			m_d3dDeviceContext->IASetVertexBuffers(0, 1, m_ModelVertexBuffer[vertexCount].GetAddressOf(), &stride, &offset);
			m_d3dDeviceContext->IASetIndexBuffer(m_ModelIndexBuffer[vertexCount].Get(), DXGI_FORMAT_R32_UINT, 0);
		
			D3D11_BUFFER_DESC indexBufferDesc;
			m_ModelIndexBuffer[vertexCount]->GetDesc(&indexBufferDesc);
			UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
			m_d3dDeviceContext->DrawIndexed(indexSize, 0, 0);
		}
		
		
		DrawImGui();
	}


	HR(m_SwapChain->Present(0, 0));
}


void LightingApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void LightingApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void LightingApp::OnMouseMove(WPARAM btnState, int x, int y)
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

void LightingApp::BuildShader()
{
	ComPtr<ID3DBlob> pVSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/color.fx", "VS", "vs_4_0", pVSBlob.GetAddressOf()));
	HR(m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA, 0}

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


void LightingApp::ChangeModelTexture(const int bodyIndex, const ComPtr<ID3D11ShaderResourceView>& newSRV)
{
	//m_ModelShaderResourceView[bodyIndex]->Release();

	m_ModelShaderResourceView[bodyIndex] = newSRV;
}


