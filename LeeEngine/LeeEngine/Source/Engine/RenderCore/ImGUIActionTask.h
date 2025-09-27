// 03.11
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "CoreMinimal.h"

struct FImGUITask
{
	std::function<void()> CommandLambda;

	FImGUITask() = default;

	FImGUITask(const FImGUITask& Other)
		: CommandLambda{Other.CommandLambda} { }
};

// 단일쓰레드(렌더링 쓰레드) 생산 - 단일쓰레드(종합 게임쓰레드) 작업
class FImGuizmoCommandPipe
{
	static Concurrency::concurrent_queue<std::shared_ptr<FImGUITask>>& GetImGuizmoCommandPipe()
	{
		static Concurrency::concurrent_queue<std::shared_ptr<FImGUITask>> ImGuizmoCommandPipe;
		return ImGuizmoCommandPipe;
	}


public:
	static void Enqueue(std::function<void()>& CommandLambda);

	static bool Dequeue(std::shared_ptr<FImGUITask>& Result);

private:
	FImGuizmoCommandPipe() = default;

	~FImGuizmoCommandPipe() = default;
};

#define ENQUEUE_IMGUI_COMMAND(Lambda) \
	{\
	std::function<void()> temp = Lambda;\
		FImGuizmoCommandPipe::Enqueue(temp);\
	}
