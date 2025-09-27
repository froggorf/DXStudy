// 03.11
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "ImGUIActionTask.h"

void FImGuizmoCommandPipe::Enqueue(std::function<void()>& CommandLambda)
{
	// 다중 생성 기반 Queue
	auto NewNode = std::make_shared<FImGUITask>();
	NewNode->CommandLambda = CommandLambda;

	GetImGuizmoCommandPipe().push(NewNode);
}

bool FImGuizmoCommandPipe::Dequeue(std::shared_ptr<FImGUITask>& Result)
{
	if (GetImGuizmoCommandPipe().try_pop(Result))
	{
		return true;
	}

	return false;
}
