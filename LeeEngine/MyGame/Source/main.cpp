//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#define _CRT_SECURE_NO_WARNINGS

#include "CoreMinimal.h"

#include "DirectX/d3dApp.h"
#include "Engine/AssetManager/AssetManager.h"
#include "ThirdParty/ImGui/imgui_internal.h"
#include "DirectX/ShadowMap.h"
#include "Engine/UEngine.h"
#include "Engine/Mesh/UStaticMesh.h"

using namespace DirectX;

class MyGame : public D3DApp
{
public:
	MyGame(HINSTANCE hInstance);
	~MyGame() override;

	// Init
	bool Init() override;
	void InitSamplerState();
	//void InitForShadowMap();
	void OnResize() override;
	void UpdateScene(float dt) override;
	//void DrawImGui() override;
	//void DrawSkeletalMesh();
	void DrawScene() override;

	// Shadow Map
	//void DrawShadowMap();

	void LoadAnimations();

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

	// TODO: DELETE_LATER
	XMMATRIX Test_DeleteLater_GetViewMatrix() const override
	{
		return m_View;
	}

	XMMATRIX Test_DeleteLater_GetProjectionMatrix() const override
	{
		return m_Proj;
	}

	XMFLOAT3 Test_DELETELATER_GetCameraPosition() const override
	{
		return m_CameraPosition;
	}
	void Test_DELETELATER_SetCameraPosition(const XMFLOAT3& NewPos) override
	{
		m_CameraPosition = NewPos;
	}

private:
	void BuildShader();

	void BuildShaderForSkeletalMesh();
	// DirectionalLight Matrix 반환하는 함수
	//void BuildShadowTransform();

	// 모델 정보
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>             m_ModelVertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>             m_ModelIndexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_ModelShaderResourceView;
	Material                                                      m_ModelMaterial;

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_BodyShaderResourceView;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_FaceShaderResourceView;

	XMFLOAT3 m_ModelPosition;
	//XMFLOAT3										m_ModelRotation;
	XMVECTOR m_ModelQuat;
	XMFLOAT3 m_ModelScale;

	// 기사 오브젝트의 본 정보		// TODO: 모듈화 시 getter 제작
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PaladinVertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_PaladinIndexBuffer;
	std::map<std::string, BoneInfo>                   m_BoneInfoMap;
	int                                               m_BoneCounter = 0;
	//std::unique_ptr<Animator>						m_PaladinAnimator;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_SkeletalMeshVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_SkeletalMeshInputLayout;

	// 큐브 출력용
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_CubeVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>             m_CubeIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CubeWaterSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CubeWireSRV;

	// 라이트 출력 용
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_SphereVertexBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_SphereIndexBuffer;

	// 그림자 맵 관련
	std::unique_ptr<ShadowMap>                 m_ShadowMap;
	XMMATRIX                                   m_LightView;
	XMMATRIX                                   m_LightProj;
	XMMATRIX                                   m_ShadowTransform;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_ShadowMapVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ShadowMapPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>       m_ShadowObjConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>       m_ShadowLightMatrixConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_ShadowSamplerState;
	float                                      m_ShadowBias;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_ShadowMapSkeletalMeshVertexShader;

	// 카메라 정보
	XMFLOAT3 m_CameraPosition;
	XMFLOAT3 m_CameraViewVector;

	// 라이트
	//DirectionalLight								m_DirectionalLight;
	//PointLight										m_PointLight;
	//SpotLight										m_SpotLight;

	// 변환 행렬
	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Proj;

	POINT m_LastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" )
#endif

	MyGame theApp(hInstance);

	if (!theApp.Init())
		return -1;

	return theApp.Run();
}

MyGame::MyGame(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	m_MainWndTitle = L"Character Animation";

	m_LastMousePos.x = 0;
	m_LastMousePos.y = 1;
	m_World          = XMMatrixIdentity();
	m_View           = XMMatrixIdentity();
	m_Proj           = XMMatrixIdentity();

	m_ModelPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//m_ModelRotation = XMFLOAT3(0.0f,0.0f,0.0f);
	m_ModelQuat  = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_ModelScale = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_CameraPosition   = XMFLOAT3(0.0f, 5.0f, -5.0f);
	m_CameraViewVector = XMFLOAT3(0.0f, -1.0f, 1.0f);
	XMStoreFloat3(&m_CameraViewVector, XMVector3Normalize(XMLoadFloat3(&m_CameraViewVector)));

	// Directional light.

	//m_DirectionalLight.Direction =  ;

	// Point Light

	// Material
	m_ModelMaterial.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_ModelMaterial.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_ModelMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
}

MyGame::~MyGame()
{
	// ComPtr
}

bool MyGame::Init()
{
	if (!D3DApp::Init())
		return false;

	InitSamplerState();

	LoadAnimations();

	BuildShader();

	BuildShaderForSkeletalMesh();

	// 그림자 맵
	//InitForShadowMap();

	return true;
}

void MyGame::LoadAnimations()
{
	// TODO: 추후 모델 관련 데이터 오브젝트로 모듈화
}

void MyGame::InitSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter             = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU           = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV           = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW           = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0]     = 0.0f; // Border 색상(흰색)
	samplerDesc.ComparisonFunc     = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD             = 0;
	samplerDesc.MaxLOD             = D3D11_FLOAT32_MAX;
	HR(GDirectXDevice->GetDevice()->CreateSamplerState(&samplerDesc, m_ShadowSamplerState.GetAddressOf()));

	D3D11_BLEND_DESC blendDesc                      = {};
	blendDesc.AlphaToCoverageEnable                 = FALSE;                        // 알파 커버리지 비활성화
	blendDesc.IndependentBlendEnable                = FALSE;                        // 독립 블렌딩 비활성화
	blendDesc.RenderTarget[0].BlendEnable           = TRUE;                         // 블렌딩 활성화
	blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;        // 소스 알파값
	blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;    // 대상 알파값의 반대
	blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;           // 더하기 연산
	blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;              // 알파값 소스
	blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;             // 알파값 대상
	blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;           // 알파값 더하기 연산
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // RGBA 모두 활성화

	ID3D11BlendState* blendState = nullptr;
	HR(GDirectXDevice->GetDevice()->CreateBlendState(&blendDesc, &blendState));

	// 파이프라인에 블렌딩 상태 설정
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // 블렌드 팩터 (기본값)
	UINT  sampleMask     = 0xFFFFFFFF;               // 샘플 마스크 (기본값)
	GDirectXDevice->GetDeviceContext()->OMSetBlendState(blendState, blendFactor, sampleMask);

	// DepthgStencilState
}

//void AnimationApp::InitForShadowMap()
//{
//
//#if defined(DEBUG) || defined(_DEBUG)
//	DebuggingSRV::InitializeDebuggingSRV(GDirectXDevice->GetDevice());
//#endif
//
//	m_ShadowBias = 0.001f;
//
//	m_ShadowMap = std::make_unique<ShadowMap>(GDirectXDevice->GetDevice(), 2048,2048);
//
//	// 그림자맵 렌더링을 위한 VS/PS 추가
//	{
//		ComPtr<ID3DBlob> pVSBlob = nullptr;
//		HR(CompileShaderFromFile(L"Shader/BuildShadowMap.hlsl", "VS_StaticMesh_ShadowMap", "vs_4_0", pVSBlob.GetAddressOf()));
//		HR(GDirectXDevice->GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_ShadowMapVertexShader.GetAddressOf()));
//
//		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
//		{
//			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
//			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA, 0}
//
//		};
//		UINT numElements = ARRAYSIZE(inputLayout);
//
//		HR(GDirectXDevice->GetDevice()->CreateInputLayout(inputLayout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_InputLayout.GetAddressOf()));
//		GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
//
//		ComPtr<ID3DBlob> pPSBlob = nullptr;
//		HR(CompileShaderFromFile(L"Shader/BuildShadowMap.hlsl", "PS", "ps_4_0", pPSBlob.GetAddressOf()));
//
//		HR(GDirectXDevice->GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_ShadowMapPixelShader.GetAddressOf()));
//
//
//		// 스켈레탈 메시를 위한 세이더 추가
//		pVSBlob->Release();		
//		HR(CompileShaderFromFile(L"Shader/BuildShadowMap.hlsl", "VS_SkeletalMesh_ShadowMap", "vs_4_0", pVSBlob.GetAddressOf()));
//		HR(GDirectXDevice->GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_ShadowMapSkeletalMeshVertexShader.GetAddressOf()));
//	}
//	
//}

void MyGame::OnResize()
{
	D3DApp::OnResize();

	// 프로젝션 매트릭스
	m_Proj = XMMatrixPerspectiveFovLH(0.5 * XM_PI, GetWindowAspectRatio(), 1.0f, 1000.0f);
}

void MyGame::UpdateScene(float dt)
{
	GEngine->Tick(dt);

	//m_PaladinAnimator->UpdateAnimation(m_Timer.DeltaTime());
}

//
//
//void AnimationApp::DrawShadowMap()
//{
//	// 디렉셔널 라이트의 변환행렬 생성
//	BuildShadowTransform();
//	m_ShadowMap->BindDsvAndSetNullRenderTarget(GDirectXDevice->GetDeviceContext());
//
//	// 큐브 그림자맵 뎁스 렌더링
//	{
//		
//		GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
//		{
//			// 셰이더 설정
//			GDirectXDevice->GetDeviceContext()->VSSetShader(m_ShadowMapVertexShader.Get(), nullptr, 0);
//			GDirectXDevice->GetDeviceContext()->PSSetShader(m_ShadowMapPixelShader.Get(), nullptr, 0);
//			// 상수버퍼 설정
//			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_ShadowLightMatrixConstantBuffer.GetAddressOf());
//			GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, m_ShadowObjConstantBuffer.GetAddressOf());
//
//			// 인풋 레이아웃
//			GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
//			GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		}
//
//		UINT stride = sizeof(MyVertexData);
//		UINT offset = 0;
//		GDirectXDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, m_CubeVertexBuffer.GetAddressOf(), &stride, &offset);
//		GDirectXDevice->GetDeviceContext()->IASetIndexBuffer(m_CubeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//
//		// cb 설정 - ShadowLightMatrixConstantBuffer
//		{
//			ShadowLightMatrixConstantBuffer slcb;
//			slcb.LightView = XMMatrixTranspose(m_LightView);
//			slcb.LightProj = XMMatrixTranspose(m_LightProj);
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_ShadowLightMatrixConstantBuffer.Get(), 0, nullptr, &slcb, 0, 0);	
//		}
//
//
//		// cb 설정 - ShadowObjConstantBuffer
//		{
//			ShadowObjConstantBuffer socb;
//			XMMATRIX world = m_World * XMMatrixScaling(15.0f,0.2f,15.0f);
//			world *= XMMatrixTranslation(0.0f,0.0f,0.0f);
//			socb.ObjWorld = XMMatrixTranspose(world);
//			
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_ShadowObjConstantBuffer.Get(), 0, nullptr, &socb, 0, 0);	
//		}
//
//		// 큐브 렌더링
//		{
//			D3D11_BUFFER_DESC indexBufferDesc;
//			m_CubeIndexBuffer->GetDesc(&indexBufferDesc);
//			UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
//			GDirectXDevice->GetDeviceContext()->DrawIndexed(indexSize, 0, 0);
//		}
//
//		// 스켈레탈 메시 버텍스 셰이더 설정
//		GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_SkeletalMeshInputLayout.Get());
//		GDirectXDevice->GetDeviceContext()->VSSetShader(m_ShadowMapSkeletalMeshVertexShader.Get(),nullptr, 0);
//		// 상수버퍼 설정
//		GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(2, 1, m_SkeletalBoneMatrixConstantBuffer.GetAddressOf());
//
//		// cb 설정 - ShadowObjConstantBuffer
//		{
//			ShadowObjConstantBuffer socb;
//			XMMATRIX world = m_World * XMMatrixScaling(m_ModelScale.x,m_ModelScale.y,m_ModelScale.z) ;
//			world *= XMMatrixRotationQuaternion(m_ModelQuat);
//			world *= XMMatrixTranslation(m_ModelPosition.x,m_ModelPosition.y,m_ModelPosition.z);
//			socb.ObjWorld = XMMatrixTranspose(world);
//
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_ShadowObjConstantBuffer.Get(), 0, nullptr, &socb, 0, 0);	
//		}
//
//
//		// cb 설정 - BoneFinalMatrices
//		{
//			SkeletalMeshBoneTransformConstantBuffer cb;
//			auto boneFinalTransforms = m_PaladinAnimator->GetFinalBoneMatrices();
//			for(int i = 0; i < MAX_BONES; ++i)
//			{
//				cb.BoneFinalTransforms[i] = XMMatrixTranspose(boneFinalTransforms[i]);
//			}
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_SkeletalBoneMatrixConstantBuffer.Get(),0, nullptr, &cb, 0,0);
//		}
//
//		// 렌더링
//		{
//			// 버텍스 버퍼에 맞춰 오브젝트 드로우
//			for(int vertexCount = 0; vertexCount < m_PaladinVertexBuffer.size(); ++vertexCount)
//			{
//				UINT stride = sizeof(MySkeletalMeshVertexData);
//				UINT offset = 0;
//				GDirectXDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, m_PaladinVertexBuffer[vertexCount].GetAddressOf(), &stride, &offset);
//				GDirectXDevice->GetDeviceContext()->IASetIndexBuffer(m_PaladinIndexBuffer[vertexCount].Get(), DXGI_FORMAT_R32_UINT, 0);
//
//				D3D11_BUFFER_DESC indexBufferDesc;
//				m_PaladinIndexBuffer[vertexCount]->GetDesc(&indexBufferDesc);
//				UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
//				GDirectXDevice->GetDeviceContext()->DrawIndexed(indexSize, 0, 0);
//			}
//			
//		}
//	}
//}
//
//void AnimationApp::DrawSkeletalMesh()
//{
//	//// 인풋 레이아웃
//	GDirectXDevice->GetDeviceContext()->IASetInputLayout(m_SkeletalMeshInputLayout.Get());
//	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// 버텍스 셰이더 (픽셀셰이더는 동일)
//	GDirectXDevice->GetDeviceContext()->VSSetShader(m_SkeletalMeshVertexShader.Get(), nullptr, 0);
//	GDirectXDevice->GetDeviceContext()->PSSetShader(m_PixelShader.Get(),nullptr,0);
//	// ConstantBuffer
//	GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_FrameConstantBuffer.GetAddressOf());
//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,m_FrameConstantBuffer.GetAddressOf());
//	GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, m_ObjConstantBuffer.GetAddressOf());
//	GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, m_SkeletalBoneMatrixConstantBuffer.GetAddressOf());
//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, m_ObjConstantBuffer.GetAddressOf());
//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, m_LightConstantBuffer.GetAddressOf());
//
//	// Frame 상수 버퍼 설정
//	{
//		// 뷰, 프로젝션 행렬
//		{
//			FrameConstantBuffer fcb;
//			fcb.View = XMMatrixTranspose(m_View);
//			fcb.Projection = XMMatrixTranspose(m_Proj);
//			fcb.LightView = XMMatrixTranspose(m_LightView);
//			fcb.LightProj = XMMatrixTranspose(m_LightProj);
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_FrameConstantBuffer.Get(), 0, nullptr, &fcb, 0, 0);
//
//		}
//
//		// 라이팅 관련
//		{
//			LightFrameConstantBuffer lfcb;
//			lfcb.gDirLight = m_DirectionalLight;
//			lfcb.gPointLight = m_PointLight;
//			// Convert Spherical to Cartesian coordinates.
//			lfcb.gEyePosW = XMFLOAT3{m_CameraPosition};
//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_LightConstantBuffer.Get(),0,nullptr,&lfcb, 0,0);
//		}
//	}
//	// 오브젝트 Draw
//	// Obj 상수 버퍼 설정
//	{
//		ObjConstantBuffer ocb;
//		XMMATRIX world = m_World * XMMatrixScaling(m_ModelScale.x,m_ModelScale.y,m_ModelScale.z);
//		//world = world * XMMatrixRotationX(m_ModelRotation.x) * XMMatrixRotationY(m_ModelRotation.y) * XMMatrixRotationZ(m_ModelRotation.z);
//		world = world * XMMatrixRotationQuaternion(m_ModelQuat);
//		world *= XMMatrixTranslation(m_ModelPosition.x,m_ModelPosition.y,m_ModelPosition.z);
//		// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
//		ocb.InvTransposeMatrix = (XMMatrixInverse(nullptr, world));
//		ocb.World = XMMatrixTranspose(world);
//
//		ocb.ObjectMaterial = m_ModelMaterial;
//
//		GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_ObjConstantBuffer.Get(), 0, nullptr, &ocb, 0, 0);	
//	}
//	{
//		SkeletalMeshBoneTransformConstantBuffer cb;
//		auto boneFinalTransforms = m_PaladinAnimator->GetFinalBoneMatrices();
//		for(int i = 0; i < MAX_BONES; ++i)
//		{
//			cb.BoneFinalTransforms[i] = XMMatrixTranspose(boneFinalTransforms[i]);
//		}
//		GDirectXDevice->GetDeviceContext()->UpdateSubresource(m_SkeletalBoneMatrixConstantBuffer.Get(),0,nullptr,&cb, 0,0);
//		
//	}
//
//
//	// Sampler State 설정
//	GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
//
//	// 버텍스 버퍼에 맞춰 오브젝트 드로우
//	for(int vertexCount = 0; vertexCount < m_PaladinVertexBuffer.size(); ++vertexCount)
//	{
//		// SRV 설정(텍스쳐)
//		{
//			GDirectXDevice->GetDeviceContext()->PSSetShaderResources(0,1, m_ModelShaderResourceView[vertexCount].GetAddressOf());	
//		}
//		UINT stride = sizeof(MySkeletalMeshVertexData);
//		UINT offset = 0;
//		GDirectXDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, m_PaladinVertexBuffer[vertexCount].GetAddressOf(), &stride, &offset);
//		GDirectXDevice->GetDeviceContext()->IASetIndexBuffer(m_PaladinIndexBuffer[vertexCount].Get(), DXGI_FORMAT_R32_UINT, 0);
//		
//		D3D11_BUFFER_DESC indexBufferDesc;
//		m_PaladinIndexBuffer[vertexCount]->GetDesc(&indexBufferDesc);
//		UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
//		GDirectXDevice->GetDeviceContext()->DrawIndexed(indexSize, 0, 0);
//	}
//
//
//}

void MyGame::DrawScene()
{
	{
		//DrawShadowMap();

		//GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

		//GEngine->Draw();

		//#if defined(DEBUG) || defined(_DEBUG)
		//		DebuggingSRV::DrawDebuggingTexture(
		//				m_d3dDeviceContext,
		//				m_ClientWidth - 500.0f, m_ClientHeight - 500.0f,
		//				450.0f, 450.0f,
		//				m_ShadowMap->GetShaderResourceViewComPtr()
		//				);
		//#endif
	}
}

void MyGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void MyGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void MyGame::BuildShader()
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth         = sizeof(FrameConstantBuffer);
	bufferDesc.Usage             = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags         = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags    = 0;

	bufferDesc.ByteWidth = sizeof(ShadowLightMatrixConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_ShadowLightMatrixConstantBuffer.GetAddressOf() ));

	bufferDesc.ByteWidth = sizeof(ShadowObjConstantBuffer);
	HR(GDirectXDevice->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_ShadowObjConstantBuffer.GetAddressOf()));
}

void MyGame::BuildShaderForSkeletalMesh()
{
	//m_d3dDeviceContext->IASetInputLayout(m_InputLayout.Get());
}

//
//void AnimationApp::BuildShadowTransform()
//{
//	float sceneRadius = 10.0f;
//	// Light View Matrix
//	XMVECTOR lightDir = XMLoadFloat3(&m_DirectionalLight.Direction);
//	XMVECTOR lightPos = -2.0f* sceneRadius * lightDir ;
//	XMVECTOR targetPos = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
//
//	// 업벡터를 구하는 연산을 진행
//	XMVECTOR up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);//ComputeUpVector(lightPos,targetPos);
//	m_LightView = XMMatrixLookAtLH(lightPos, targetPos,up);
//	
//	
//	// Light Proj
//	float left = -sceneRadius;
//	float right = sceneRadius;
//	float bottom = -sceneRadius;
//	float top = sceneRadius;
//	float nearZ = 0.01f;
//	float farZ = sceneRadius*10;
//	m_LightProj = XMMatrixOrthographicLH(sceneRadius*2, sceneRadius*2, nearZ,farZ);
//}
