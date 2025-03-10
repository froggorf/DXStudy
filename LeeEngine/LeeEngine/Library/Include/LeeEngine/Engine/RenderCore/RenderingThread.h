// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include <functional>
#include <concurrent_queue.h>
#include <ppltasks.h>
#include <thread>

#include "Engine/MyEngineUtils.h"
#include "Engine/UEditorEngine.h"
#include "Engine/DirectX/Device.h"
#include "ThirdParty/ImGui/backends/imgui_impl_dx11.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue

class FPrimitiveSceneProxy;

struct FRenderTask
{
	UINT PrimitiveID;
	std::function<void(std::shared_ptr<class FScene>&)> CommandLambda;
	FRenderTask* Next;
	FRenderTask() : Next{ nullptr } {}
	FRenderTask(const FRenderTask& Other) : PrimitiveID{ Other.PrimitiveID }, CommandLambda{ Other.CommandLambda }, Next{ Other.Next } {}
};

class FRenderCommandPipe
{
private:
	static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>> RenderCommandPipe;
	//static std::unique_ptr<FRenderCommandPipe> RenderCommandPipe;
	//std::atomic<Node*> Head;
	//std::atomic<Node*> Tail;

public:
	static void Enqueue(std::function<void(std::shared_ptr<class FScene>&)>& CommandLambda)
	{
		// 다중 생성 기반 Queue
		std::shared_ptr<FRenderTask> NewNode = std::make_shared<FRenderTask>();
		//Node* NewNode = new Node();
		NewNode->CommandLambda = CommandLambda;

		RenderCommandPipe.push(NewNode);
		//Node* PrevHead = RenderCommandPipe->Head.exchange(NewNode);
		//PrevHead->Next= NewNode;
	}
	static bool Dequeue(std::shared_ptr<FRenderTask>& Result)
	{
		if(RenderCommandPipe.try_pop(Result))
		{
			return true;
		}

		return false;

		// 단일 소비 기반 queue
		//Node* PrevTail = Tail.load();
		//Node* Next = PrevTail->Next;
		//if (Next == nullptr)
		//{
		//	return false;
		//}
		//Result = *Next;
		//Tail.store(Next);
		//delete PrevTail;
		//return true;

	}

protected:
private:
public:
protected:
private:
	FRenderCommandPipe() = default;

	~FRenderCommandPipe() = default;
};

#define ENQUEUE_RENDER_COMMAND(Lambda) \
	std::function<void(std::shared_ptr<FScene>&)> temp = Lambda;\
		FRenderCommandPipe::Enqueue(temp);

inline bool bIsGameKill = false;
inline UINT RenderingThreadFrameCount = 0;

// 렌더링에 대한 정보를 가지고 있는 클래스 (씬 단위)
// 03.10 렌더링 쓰레드의 경우 단일 소비로 진행할 예정이므로
// 멀티쓰레드 동기화에 대한 처리 x
class FScene
{
public:
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PrimitiveSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingAddSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingDeleteSceneProxies;

	bool bIsFrameStart;
protected:
public:

	static void KillRenderingThread()
	{
		bIsGameKill = true;
	}

	static void InitSceneData_GameThread()
	{ 
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				FScene::InitSceneData_RenderThread(SceneData);
			}
		)
	}

	static void InitSceneData_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData)
		{
			SceneData.reset();
		}
		SceneData = std::make_shared<FScene>();
	}

	static void BeginRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				BeginRenderFrame_RenderThread(SceneData);	
			}
		)
	}
	static void BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData->bIsFrameStart)
		{
			//MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
			return;
		}

		++RenderingThreadFrameCount;
		SceneData->bIsFrameStart = true;
		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;
			MY_LOG("SceneProxy Add New Proxy - PrimitiveID = " + std::to_string(NewPrimitiveProxy.first), EDebugLogLevel::DLL_Display, "");
		}
		SceneData->PendingAddSceneProxies.clear();

		
	}

	static void EndRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				EndRenderFrame_RenderThread(SceneData);	
			}
		)
	}
	static void EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

		}
		SceneData->PendingAddSceneProxies.clear();
		SceneData->bIsFrameStart = false;
	}

	static void AddPrimitive_GameThread(UINT PrimitiveID, std::shared_ptr<FPrimitiveSceneProxy>& SceneProxy)
	{
		if(nullptr == SceneProxy)
		{
			return;
		}
		auto Lambda = 
			[PrimitiveID, SceneProxy](std::shared_ptr<FScene>& SceneData)
			{
				FScene::AddPrimitive_RenderThread(SceneData, PrimitiveID, SceneProxy);
			};
		ENQUEUE_RENDER_COMMAND(Lambda)
	}
	static void AddPrimitive_RenderThread(const std::shared_ptr<FScene>& SceneData, UINT PrimitiveID, const std::shared_ptr<FPrimitiveSceneProxy>& NewProxy)
	{
		if(!SceneData)
		{
			MY_LOG("SceneDataError", EDebugLogLevel::DLL_Error, "No SceneData");
			return;
		}
		SceneData->PendingAddSceneProxies[PrimitiveID] = NewProxy;
	}

	static void DrawScene_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
		{
			FScene::DrawScene_RenderThread(SceneData);
		})
		
	}
	static void DrawScene_RenderThread(std::shared_ptr<FScene> SceneData)
	{
		// 프레임 단위 세팅
		{
			std::cout<<"FrameRenderCount" << RenderingThreadFrameCount << std::endl;

			GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
			GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());

			const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
			GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), clearColor);
			GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

			GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
			{
				// 셰이더 설정
				GDirectXDevice->GetDeviceContext()->VSSetShader(GDirectXDevice->GetStaticMeshVertexShader().Get(), nullptr, 0);
				GDirectXDevice->GetDeviceContext()->PSSetShader(GDirectXDevice->GetTexturePixelShader().Get(), nullptr, 0);
				// 상수버퍼 설정
				GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, GDirectXDevice->GetLightConstantBuffer().GetAddressOf());


				// 인풋 레이아웃
				GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
				GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// Frame 상수 버퍼 설정
				{
					// 뷰, 프로젝션 행렬
					{
						FrameConstantBuffer fcb;
						// TODO: 카메라 구현 시 수정
						XMFLOAT3 m_CameraPosition = XMFLOAT3(0.0f,5.0f,-5.0f);
						XMFLOAT3 m_CameraViewVector = XMFLOAT3(0.0f,-1.0f,1.0f);
						XMMATRIX ViewMat = XMMatrixLookToLH(XMLoadFloat3(&m_CameraPosition), XMLoadFloat3(&m_CameraViewVector), XMVectorSet(0.0f,1.0f,0.0f,0.0f));

						fcb.View = XMMatrixTranspose(ViewMat);
						// TODO: 카메라 구현 시 수정
						XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);
						fcb.Projection = XMMatrixTranspose(ProjMat);
						fcb.LightView = XMMatrixTranspose(XMMatrixIdentity());//m_LightView
						fcb.LightProj = XMMatrixTranspose(XMMatrixIdentity()); //m_LightProj
						GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetFrameConstantBuffer().Get(), 0, nullptr, &fcb, 0, 0);

					}

					// 라이팅 관련
					{
						LightFrameConstantBuffer lfcb;
						DirectionalLight TempDirectionalLight;
						TempDirectionalLight.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
						TempDirectionalLight.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
						TempDirectionalLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
						XMStoreFloat3(&TempDirectionalLight.Direction, XMVector3Normalize(XMVectorSet(0.57735f, -0.57735f, 0.57735f,0.0f)));
						lfcb.gDirLight = TempDirectionalLight;

						PointLight TempPointLight;
						TempPointLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
						//m_PointLight.Position = XMFLOAT3(796.5f, 700.0f, 0.7549f);
						TempPointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
						TempPointLight.Range    = 0.0f;
						lfcb.gPointLight = TempPointLight;
						// Convert Spherical to Cartesian coordinates.
						lfcb.gEyePosW = XMFLOAT3{GEngine->Test_DeleteLater_GetCameraPosition()};
						GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetLightConstantBuffer().Get(),0,nullptr,&lfcb, 0,0);
					}
				}




				// Sampler State 설정
				GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, GDirectXDevice->GetSamplerState().GetAddressOf());

				//if(CurrentWorld)
				//{
				//	CurrentWorld->TestDrawWorld();
				//}
				//DrawImGui();

			}
		
		}
		
		HR(GDirectXDevice->GetSwapChain()->Present(0, 0));

		// SceneProxy Render
		EndRenderFrame_RenderThread(SceneData);
	}
public:
protected:
private:
};


// 엔진 초기화 시 생성되며
// RenderCommandPipe에 존재하는 Command를 계속해서 수행하는 클래스
class FRenderCommandExecutor
{
public:
	static void Execute()
	{
		std::shared_ptr<FRenderTask> Task;
		while(true)
		{
			if(bIsGameKill)
			{
				return;
			}

			if(FRenderCommandPipe::Dequeue(Task))
			{
				Task->CommandLambda(CurrentSceneData);
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	//// 반드시 RenderCommandPipe를 통해 호출해야함
	//// ENQUE_RENDER_COMMAND(...
	//static void InitNewSceneData()
	//{
	//	if(CurrentSceneData)
	//	{
	//		CurrentSceneData.release();
	//	}

	//	CurrentSceneData = std::make_unique<FScene>();
	//}
protected:
private:
public:
protected:
private:
	static std::shared_ptr<FScene> CurrentSceneData;
};