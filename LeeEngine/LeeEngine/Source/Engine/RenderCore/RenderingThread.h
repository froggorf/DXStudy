// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include <functional>
#include <thread>

#include "Engine/MyEngineUtils.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue
class FRenderCommandPipe
{
private:
	struct Node
	{
		UINT PrimitiveID;
		std::function<void(int)> CommandLambda;
		Node* Next;
		Node() : Next{ nullptr } {}
		Node(const Node& Other) : PrimitiveID{ Other.PrimitiveID }, CommandLambda{ Other.CommandLambda }, Next{ Other.Next } {}
	};
	static std::unique_ptr<FRenderCommandPipe> RenderCommandPipe;
	std::atomic<Node*> Head;
	std::atomic<Node*> Tail;

public:
	static void Enqueue(UINT PrimitiveId, const std::function<void(int)>& CommandLambda)
	{
		// 다중 생성 기반 Queue
		Node* NewNode = new Node();
		NewNode->PrimitiveID = PrimitiveId;
		NewNode->CommandLambda = CommandLambda;

		Node* PrevHead = RenderCommandPipe->Head.exchange(NewNode);

		PrevHead->Next= NewNode;
	}
	bool Dequeue(Node& Result)
	{
		// 단일 소비 기반 queue
		Node* PrevTail = Tail.load();
		Node* Next = PrevTail->Next;
		if (Next == nullptr)
		{
			return false;
		}
		Result = *Next;
		Tail.store(Next);
		delete PrevTail;
		return true;
	}

protected:
private:
public:
protected:
private:
	FRenderCommandPipe()
	{
		Node* DummyNode = new Node();
		Head.store(DummyNode);
		Tail.store(DummyNode);
	}
	~FRenderCommandPipe()
	{
		while (Node* Node = Head.load())
		{
			Head.store(Node->Next);
			delete Node;
		}
	}
};

#define ENQUEUE_RENDER_COMMAND(PrimitiveID, Lambda) \
		FRenderCommandPipe::Enqueue(PrimitiveID, Lambda);
