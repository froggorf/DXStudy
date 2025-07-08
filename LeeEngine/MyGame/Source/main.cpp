//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
//#define _CRT_SECURE_NO_WARNINGS
#ifdef _DEBUG
#pragma message("_DEBUG defined")
#endif
#ifdef NDEBUG
#pragma message("NDEBUG defined")
#endif

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
	//void InitForShadowMap();
	void OnResize() override;
	void UpdateScene(float dt) override;
	//void DrawImGui() override;
	//void DrawSkeletalMesh();
	void DrawScene() override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

private:

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
}

MyGame::~MyGame()
{
	// ComPtr
}

bool MyGame::Init()
{
	if (!D3DApp::Init())
		return false;

	return true;
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
}

void MyGame::UpdateScene(float dt)
{
	GEngine->Tick(dt);
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
