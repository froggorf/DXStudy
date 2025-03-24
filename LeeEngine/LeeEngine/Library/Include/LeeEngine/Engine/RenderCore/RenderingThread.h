// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include <functional>
#include <concurrent_queue.h>
#include <ppltasks.h>
#include <thread>

#include "ImGUIActionTask.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/SceneView.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/DirectX/Device.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/SceneProxy/FPrimitiveSceneProxy.h"
#include "ThirdParty/ImGui/backends/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/backends/imgui_impl_win32.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue

class USceneComponent;
class AActor;

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
	static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>>& GetRenderCommandPipe()
	{
		static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>> RenderCommandPipe;
		return RenderCommandPipe;
	}
	
	//static std::unique_ptr<FRenderCommandPipe> RenderCommandPipe;
	//std::atomic<Node*> Head;
	//std::atomic<Node*> Tail;

public:
	static void Enqueue(std::function<void(std::shared_ptr<class FScene>&)>& CommandLambda)
	{
		// 다중 생성 기반 Queue
		std::shared_ptr<FRenderTask> NewNode = std::make_shared<FRenderTask>();
		NewNode->CommandLambda = CommandLambda;

		GetRenderCommandPipe().push(NewNode);
		// MPSC Queue
		//Node* NewNode = new Node();
		//Node* PrevHead = RenderCommandPipe->Head.exchange(NewNode);
		//PrevHead->Next= NewNode;
	}
	static bool Dequeue(std::shared_ptr<FRenderTask>& Result)
	{
		if(GetRenderCommandPipe().try_pop(Result))
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

inline std::atomic<bool> bIsGameKill = false;
inline std::atomic<UINT> RenderingThreadFrameCount = 0;


// 렌더링에 대한 정보를 가지고 있는 클래스 (씬 단위)
// 03.10 렌더링 쓰레드의 경우 단일 소비로 진행할 예정이므로
// 멀티쓰레드 동기화에 대한 처리 x
class FScene
{
public:
	FScene(){}
	virtual ~FScene(){}
	// ==================== FPrimitiveSceneProxy ====================
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PrimitiveSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingAddSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingDeleteSceneProxies;

	std::map<UINT, FTransform> PendingNewTransformProxies;
	// ==================== FPrimitiveSceneProxy ====================

	
	bool bMustResetLevelDataAtEndFrame = false;
	bool bIsFrameStart;
protected:
public:
	// 엔진 종료 시 렌더링 쓰레드를 죽이는 함수
	static void KillRenderingThread()
	{
		bIsGameKill = true;
	}
	// ImGUI 종료 함수
	static void ShutdownImgui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	// 게임쓰레드 _ 레벨마다 가진 씬 데이터를 Init하는 함수 
	static void InitSceneData_GameThread()
	{ 
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				//Scene::InitSceneData_RenderThread(SceneData);
				SceneData->bMustResetLevelDataAtEndFrame = true;
			}
		)
	}
	// InitSceneData_GameThread()
	static void InitSceneData_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		
	}
	virtual void InitLevelData();

	// 렌더쓰레드 프레임 시작 알림 함수
	// 게임쓰레드 시작 시 호출
	static void BeginRenderFrame_GameThread(UINT GameThreadFrameCount)
	{
		ENQUEUE_RENDER_COMMAND([GameThreadFrameCount](std::shared_ptr<FScene>& SceneData)
			{
				BeginRenderFrame_RenderThread(SceneData, GameThreadFrameCount);	
			}
		)
	}
	static void BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount);
	virtual void BeginRenderFrame();

	// 렌더 쓰레드 프레임 종료 함수 (Draw에서 호출)
	static void EndRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				EndRenderFrame_RenderThread(SceneData);	
			}
		)
	}

	// 새로운 UPrimitiveComponent 생성 후 register 시 렌더링 쓰레드에게 알리는 함수
	static void AddPrimitive_GameThread(UINT PrimitiveID, std::shared_ptr<FPrimitiveSceneProxy>& SceneProxy, FTransform InitTransform)
	{
		if(nullptr == SceneProxy)
		{
			return;
		}
		auto Lambda = 
			[PrimitiveID, SceneProxy, InitTransform](std::shared_ptr<FScene>& SceneData)
			{
				FScene::AddPrimitive_RenderThread(SceneData, PrimitiveID, SceneProxy);
			};
		ENQUEUE_RENDER_COMMAND(Lambda)
	}
	static void AddPrimitive_RenderThread(const std::shared_ptr<FScene>& SceneData, UINT PrimitiveID, const std::shared_ptr<FPrimitiveSceneProxy>& NewProxy)
	{
		if(!SceneData)
		{
			/*MY_LOG("SceneDataError", EDebugLogLevel::DLL_Error, "No SceneData");*/
			return;
		}
		SceneData->PendingAddSceneProxies[PrimitiveID] = NewProxy;
	}

	static void NewTransformToPrimitive_GameThread(UINT PrimitiveID, const FTransform& NewTransform)
	{
		if(PrimitiveID > 0 )
		{
			auto Lambda = [PrimitiveID, NewTransform](std::shared_ptr<FScene>& SceneData)
				{
					SceneData->PendingNewTransformProxies[PrimitiveID] = NewTransform;
				};
			ENQUEUE_RENDER_COMMAND(Lambda)	
		}
		
	}

	static void UpdateSkeletalMeshAnimation_GameThread(UINT PrimitiveID, const std::vector<XMMATRIX>& FinalMatrices);

	// 게임쓰레드 호출_ 씬 렌더링 요청 함수
	static void DrawScene_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
		{
			FScene::DrawScene_RenderThread(SceneData);
			
		})
		
	}

	
	static void DrawScene_RenderThread(std::shared_ptr<FScene> SceneData);
	virtual void SetDrawScenePipeline(const float* ClearColor);
	virtual void AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData){}

	virtual XMMATRIX GetViewMatrix();
	virtual XMMATRIX GetProjectionMatrix();
public:
protected:
private:
	static void EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData);
};


// 엔진 초기화 시 생성되며
// RenderCommandPipe에 존재하는 Command를 계속해서 수행하는 클래스
class FRenderCommandExecutor
{
public:
	static void Execute(const std::shared_ptr<FScene>& InSceneData)
	{
		if(!CurrentSceneData)
		{
			CurrentSceneData = std::move(InSceneData);
			
		}
		
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
protected:
private:
public:
	static std::shared_ptr<FScene> CurrentSceneData;
protected:
private:

};