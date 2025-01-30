//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#include <format>
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
	XMMATRIX InvTransposeMatrix;
	Material ObjectMaterial;
};

struct LightFrameConstantBuffer
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
    XMFLOAT3 gEyePosW;
    float pad;
};

class AnimationApp : public D3DApp
{
public:
	AnimationApp(HINSTANCE hInstance);
	~AnimationApp();

	
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
	Material										m_ModelMaterial;

	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_BodyShaderResourceView;
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_FaceShaderResourceView;

	XMFLOAT3										m_ModelPosition;
	//XMFLOAT3										m_ModelRotation;
	XMVECTOR										m_ModelQuat;
	XMFLOAT3										m_ModelScale;

	// 라이트 출력 용
	std::vector<ComPtr<ID3D11Buffer>>				m_SphereVertexBuffer;
	std::vector<ComPtr<ID3D11Buffer>>				m_SphereIndexBuffer;

	// 카메라 정보
	XMFLOAT3										m_CameraPosition;
	XMFLOAT3										m_CameraViewVector;

	// 라이트
	DirectionalLight								m_DirectionalLight;
	PointLight										m_PointLight;
	SpotLight										m_SpotLight;

	// 파이프라인
	ComPtr<ID3D11VertexShader>	m_VertexShader;
	ComPtr<ID3D11PixelShader>	m_PixelShader;
	ComPtr<ID3D11InputLayout>	m_InputLayout;
	ComPtr<ID3D11SamplerState>	m_SamplerState;

	// 상수버퍼
	ComPtr<ID3D11Buffer>		m_FrameConstantBuffer;
	ComPtr<ID3D11Buffer>		m_ObjConstantBuffer;
	ComPtr<ID3D11Buffer>		m_LightConstantBuffer;

	// 변환 행렬
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
	AllocConsole();
	FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

	AnimationApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

AnimationApp::AnimationApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
	m_MainWndTitle = L"Texture To Model";

	m_LastMousePos.x = 0; m_LastMousePos.y = 1;
	m_World = XMMatrixIdentity();
	m_View = XMMatrixIdentity();
	m_Proj = XMMatrixIdentity();

	m_ModelPosition = XMFLOAT3(0.0f,0.0f,0.0f);
	//m_ModelRotation = XMFLOAT3(0.0f,0.0f,0.0f);
	m_ModelQuat = XMQuaternionIdentity();
	m_ModelScale = XMFLOAT3(0.0f,0.0f,0.0f);

	m_CameraPosition = XMFLOAT3(0.0f,3.0f,-5.0f);
	m_CameraViewVector = XMFLOAT3(0.0f,0.0f,1.0f);
	
	// Directional light.
	m_DirectionalLight.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirectionalLight.Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirectionalLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirectionalLight.Direction = XMFLOAT3(0.0f,-1.0f,0.0f);

	// Point Light
	m_PointLight.Ambient  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_PointLight.Diffuse  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_PointLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
	//m_PointLight.Position = XMFLOAT3(796.5f, 700.0f, 0.7549f);
	m_PointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
	m_PointLight.Range    = 100.0f;

	// Material
	m_ModelMaterial.Ambient  = XMFLOAT4(1.0f,1.0f,1.0f, 1.0f);
	m_ModelMaterial.Diffuse  = XMFLOAT4(1.0f,1.0f,1.0f, 1.0f);
	m_ModelMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
}

AnimationApp::~AnimationApp()
{
	// ComPtr
}

bool AnimationApp::Init()
{
	if(!D3DApp::Init())
		return false;

	InitSamplerState();

	LoadModels();

	BuildShader();

	return true;
}

void AnimationApp::LoadModels()
{
	AssetManager::LoadModelData("Model/Racco.fbx", m_d3dDevice, m_ModelVertexBuffer, m_ModelIndexBuffer);

	AssetManager::LoadModelData("Model/Sphere.obj", m_d3dDevice, m_SphereVertexBuffer, m_SphereIndexBuffer);

	m_ModelPosition = XMFLOAT3(0.0f,0.0f,0.0f);
	m_ModelScale = XMFLOAT3(0.02f,0.02f,0.02f);
	//m_ModelRotation = XMFLOAT3(0.0f,0.0f,0.0f);

	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_01.png", m_d3dDevice, m_ModelShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_01.png", m_d3dDevice, m_ModelShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_01.png", m_d3dDevice, m_BodyShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_02.png", m_d3dDevice, m_BodyShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Cat_06.png", m_d3dDevice, m_BodyShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_01.png", m_d3dDevice, m_FaceShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_04.png", m_d3dDevice, m_FaceShaderResourceView);
	//AssetManager::LoadTextureFromFile(L"Texture/T_Chibi_Emo_25.png", m_d3dDevice, m_FaceShaderResourceView);
	//m_ModelShaderResourceView.push_back(m_BodyShaderResourceView[2]);
	//m_ModelShaderResourceView.push_back(m_FaceShaderResourceView[2]);
	AssetManager::LoadTextureFromFile(L"Texture/T_Racco_A.png", m_d3dDevice,m_BodyShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Racco_B.png", m_d3dDevice,m_BodyShaderResourceView);
	AssetManager::LoadTextureFromFile(L"Texture/T_Racco_C.png", m_d3dDevice,m_BodyShaderResourceView);
	m_ModelShaderResourceView.push_back(m_BodyShaderResourceView[2]);
}


void AnimationApp::InitSamplerState()
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

void AnimationApp::OnResize()
{
	D3DApp::OnResize();

	// 프로젝션 매트릭스
	m_Proj = XMMatrixPerspectiveFovLH(0.5*XM_PI, GetWindowAspectRatio(), 1.0f, 1000.0f);
}

void AnimationApp::UpdateScene(float dt)
{
    m_View = XMMatrixLookAtLH(XMLoadFloat3(&m_CameraPosition), XMLoadFloat3(&m_CameraViewVector), XMVectorSet(0.0f,1.0f,0.0f,0.0f));
}

void AnimationApp::DrawImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// UI 코드 작성
	ImGui::SetNextWindowSize(ImVec2(600,200));
    ImGui::Begin("Controller");
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
    ImGui::End();

	ImGui::SetNextWindowSize(ImVec2{600.0f,100.0f});
	ImGui::Begin("Color");
	static float color[] = {1.0f,1.0f,1.0f,1.0f};
	if(ImGui::ColorEdit4("Directional Light Color", color, 0))
	{
		m_DirectionalLight.Ambient = XMFLOAT4(color[0],color[1],color[2],color[3]);
	}

	ImGui::SliderFloat3("PointLight", (float*)&m_PointLight.Position, -5.0f,5.0f);
	static float plColor[] = {m_PointLight.Ambient.x,m_PointLight.Ambient.y,m_PointLight.Ambient.z,1.0f};
	if(ImGui::ColorEdit4("PointLight Color", plColor, 0))
	{
		m_PointLight.Ambient = XMFLOAT4(plColor[0],plColor[1],plColor[2],plColor[3]);
		m_PointLight.Diffuse= XMFLOAT4(plColor[0],plColor[1],plColor[2],plColor[3]);
		m_PointLight.Specular= XMFLOAT4(1.0f,1.0f,1.0f,1.0f);//XMFLOAT4(plColor[0],plColor[1],plColor[2],plColor[3]);
	}
	ImGui::End();

	// ImGuizmo
	{
		ImGuizmo::BeginFrame();
		ImGuiIO& io = ImGui::GetIO();
		
		ImGui::SliderFloat3("Translation", (float*)&m_ModelPosition, -5.0f, 5.0f );

		static float rotationDegrees[] = {0.0f,0.0f,0.0f};
		if(ImGui::SliderFloat3("Rotation", (float*)&rotationDegrees,-180.0f,180.0f))
		{
			
			m_ModelQuat = XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(rotationDegrees[0]),
				XMConvertToRadians(rotationDegrees[1]),
				XMConvertToRadians(rotationDegrees[2]));
			XMQuaternionNormalize(m_ModelQuat);
		}
		ImGui::SliderFloat3("Scale", (float*)&m_ModelScale, -5.0f, 5.0f);
		ImGuizmo::SetRect(0,0,io.DisplaySize.x,io.DisplaySize.y);
		static bool bUseGizmo = false;
		ImGui::Checkbox("Use Gizmo", &bUseGizmo);
		XMMATRIX identityMat = XMMatrixIdentity();

		if (bUseGizmo)
		{
			static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
			static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q))
			{
				std::cout << " Q" << std::endl;
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_W))
			{
				std::cout << "R" << std::endl;
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			}
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_E))
			{
				std::cout << "R" << std::endl;
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			}
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_1))
			{
				std::cout<<"WORLD"<<std::endl;
				mCurrentGizmoMode = ImGuizmo::WORLD;
			}
			if(ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_2))
			{
				std::cout<<"Local"<<std::endl;
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			}

			XMMATRIX objectMatrix;
			XMMATRIX deltaMatrix;
			XMFLOAT3 test {0.0f,0.0f,0.0f};
			// Position, Rotation, Scale -> Matrix
			ImGuizmo::RecomposeMatrixFromComponents(reinterpret_cast<float*>(&m_ModelPosition), reinterpret_cast<float*>(&test), reinterpret_cast<float*>(&m_ModelScale), reinterpret_cast<float*>(&objectMatrix));

			// 이게 기즈모를 통한 트랜스레이션을 적용시켜주는 함수
			//ImGuizmo::Manipulate(reinterpret_cast<float*>(&m_View), reinterpret_cast<float*>(&m_Proj), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(&objectMatrix), (float*)&deltaMatrix, NULL);
			ImGuizmo::Manipulate(reinterpret_cast<float*>(&m_View), reinterpret_cast<float*>(&m_Proj),
				mCurrentGizmoOperation, mCurrentGizmoMode, (float*)&objectMatrix, (float*)&deltaMatrix, nullptr);

			ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&objectMatrix), (float*)&m_ModelPosition, nullptr, (float*)&m_ModelScale);

			XMFLOAT3 deltaRot;
			ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&deltaMatrix), nullptr, (float*)&deltaRot, nullptr);

			rotationDegrees[0] = std::fmod (rotationDegrees[0] + deltaRot.x , 180.0f);
			rotationDegrees[1] = std::fmod (rotationDegrees[1] + deltaRot.y , 180.0f);
			rotationDegrees[2] = std::fmod (rotationDegrees[2] + deltaRot.z , 180.0f);
			XMVECTOR deltaQuat = XMQuaternionRotationRollPitchYaw( 
									XMConvertToRadians(deltaRot.x),
								XMConvertToRadians(deltaRot.y),
								XMConvertToRadians(deltaRot.z));

			m_ModelQuat = XMQuaternionMultiply(m_ModelQuat , deltaQuat) ;
			XMQuaternionNormalize(m_ModelQuat);
		}

	}


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void AnimationApp::DrawScene()
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
		m_d3dDeviceContext->PSSetConstantBuffers(1,1, m_ObjConstantBuffer.GetAddressOf());
		m_d3dDeviceContext->PSSetConstantBuffers(2,1, m_LightConstantBuffer.GetAddressOf());

		// 인풋 레이아웃
		m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());
		m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Frame 상수 버퍼 설정
		{
			// 뷰, 프로젝션 행렬
			{
				FrameConstantBuffer fcb;
				fcb.View = XMMatrixTranspose(m_View);
				fcb.Projection = XMMatrixTranspose(m_Proj);
				m_d3dDeviceContext->UpdateSubresource(m_FrameConstantBuffer.Get(), 0, nullptr, &fcb, 0, 0);
			}

			// 라이팅 관련
			{
				LightFrameConstantBuffer lfcb;
				lfcb.gDirLight = m_DirectionalLight;
				lfcb.gPointLight = m_PointLight;
				// Convert Spherical to Cartesian coordinates.
				lfcb.gEyePosW = XMFLOAT3{m_CameraPosition};
				m_d3dDeviceContext->UpdateSubresource(m_LightConstantBuffer.Get(),0,nullptr,&lfcb, 0,0);
			}
		}
		
		
		// 오브젝트 Draw
		// Obj 상수 버퍼 설정
		{
			ObjConstantBuffer ocb;
			XMMATRIX world = m_World * XMMatrixScaling(m_ModelScale.x,m_ModelScale.y,m_ModelScale.z);
			//world = world * XMMatrixRotationX(m_ModelRotation.x) * XMMatrixRotationY(m_ModelRotation.y) * XMMatrixRotationZ(m_ModelRotation.z);
			world = world * XMMatrixRotationQuaternion(m_ModelQuat);
			world *= XMMatrixTranslation(m_ModelPosition.x,m_ModelPosition.y,m_ModelPosition.z);
			// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
			ocb.InvTransposeMatrix = (XMMatrixInverse(nullptr, world));
			ocb.World = XMMatrixTranspose(world);

			ocb.ObjectMaterial = m_ModelMaterial;
			m_d3dDeviceContext->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);	
		}
		

		// Sampler State 설정
		m_d3dDeviceContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());

		// 버텍스 버퍼에 맞춰 오브젝트 드로우
		for(int vertexCount = 0; vertexCount < m_ModelVertexBuffer.size(); ++vertexCount)
		{
			// SRV 설정(텍스쳐)
			{
				m_d3dDeviceContext->PSSetShaderResources(0,1, m_ModelShaderResourceView[vertexCount].GetAddressOf());	
			}
			UINT stride = sizeof(MyVertexData);
			UINT offset = 0;
			m_d3dDeviceContext->IASetVertexBuffers(0, 1, m_ModelVertexBuffer[vertexCount].GetAddressOf(), &stride, &offset);
			m_d3dDeviceContext->IASetIndexBuffer(m_ModelIndexBuffer[vertexCount].Get(), DXGI_FORMAT_R32_UINT, 0);
		
			D3D11_BUFFER_DESC indexBufferDesc;
			m_ModelIndexBuffer[vertexCount]->GetDesc(&indexBufferDesc);
			UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
			m_d3dDeviceContext->DrawIndexed(indexSize, 0, 0);
		}

		// 테스트용 스피어 드로우

		// Obj 상수 버퍼 설정
		{
			ObjConstantBuffer ocb;
			XMMATRIX world = m_World * XMMatrixScaling(0.15f,0.15f,0.15f);
			//world *= XMMatrixTranslation(0.0f, -2.5f, 0.0f);
			world *= XMMatrixTranslation(m_PointLight.Position.x,m_PointLight.Position.y,m_PointLight.Position.z);
			ocb.World = XMMatrixTranspose(world);
			// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
			ocb.InvTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
			ocb.ObjectMaterial = m_ModelMaterial;
			m_d3dDeviceContext->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);	
		}

		{
			m_d3dDeviceContext->PSSetShaderResources(0,1,m_ModelShaderResourceView[0].GetAddressOf());
			UINT stride = sizeof(MyVertexData);
			UINT offset = 0;
			m_d3dDeviceContext->IASetVertexBuffers(0, 1, m_SphereVertexBuffer[0].GetAddressOf(), &stride, &offset);
			m_d3dDeviceContext->IASetIndexBuffer(m_SphereIndexBuffer[0].Get(), DXGI_FORMAT_R32_UINT, 0);
		
			D3D11_BUFFER_DESC indexBufferDesc;
			m_SphereIndexBuffer[0]->GetDesc(&indexBufferDesc);
			UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
			m_d3dDeviceContext->DrawIndexed(indexSize, 0, 0);
		}

		DrawImGui();
	}

	HR(m_SwapChain->Present(0, 0));
}


void AnimationApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void AnimationApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void AnimationApp::OnMouseMove(WPARAM btnState, int x, int y)
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

void AnimationApp::BuildShader()
{
	ComPtr<ID3DBlob> pVSBlob = nullptr;
	HR(CompileShaderFromFile(L"Shader/LightColor.hlsl", "VS", "vs_4_0", pVSBlob.GetAddressOf()));
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
	HR(CompileShaderFromFile(L"Shader/LightColor.hlsl", "PS", "ps_4_0", pPSBlob.GetAddressOf()));

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

	bufferDesc.ByteWidth = sizeof(LightFrameConstantBuffer);
	HR(m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_LightConstantBuffer.GetAddressOf()))

}


void AnimationApp::ChangeModelTexture(const int bodyIndex, const ComPtr<ID3D11ShaderResourceView>& newSRV)
{
	//m_ModelShaderResourceView[bodyIndex]->Release();

	m_ModelShaderResourceView[bodyIndex] = newSRV;
}


