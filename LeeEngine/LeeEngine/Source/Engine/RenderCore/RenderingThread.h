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
	static void Enqueue(UINT PrimitiveId, std::function<void(std::shared_ptr<class FScene>&)>& CommandLambda)
	{
		// 다중 생성 기반 Queue
		std::shared_ptr<FRenderTask> NewNode = std::make_shared<FRenderTask>();
		//Node* NewNode = new Node();
		NewNode->PrimitiveID = PrimitiveId;
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

#define ENQUEUE_RENDER_COMMAND(PrimitiveID, Lambda) \
		FRenderCommandPipe::Enqueue(PrimitiveID, Lambda);



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
private:

	static void InitSceneData_GameThread()
	{
	}

	static void InitSceneData_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData)
		{
			SceneData.reset();
		}
		SceneData = std::make_shared<FScene>();
	}

	static void BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData->bIsFrameStart)
		{
			MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
			return;
		}
		SceneData->bIsFrameStart = true;
		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;
		}
		SceneData->PendingAddSceneProxies.clear();

		
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
	static void AddPrimitive_RenderThread(const std::shared_ptr<FScene>& SceneData, UINT PrimitiveID, std::shared_ptr<FPrimitiveSceneProxy>& NewProxy)
	{
		if(!SceneData)
		{
			MY_LOG("SceneDataError", EDebugLogLevel::DLL_Error, "No SceneData");
		}
		SceneData->PendingAddSceneProxies[PrimitiveID] = NewProxy;
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