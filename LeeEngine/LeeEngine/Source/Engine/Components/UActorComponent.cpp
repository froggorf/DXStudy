// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UActorComponent.h"

static UINT ComponentIDCount = 0;

UActorComponent::UActorComponent()
{
	ComponentID = ComponentIDCount++;
}

void UActorComponent::TickComponent(float DeltaSeconds)
{
}

#ifdef WITH_EDITOR
void UActorComponent::DrawDetailPanel(UINT ComponentDepth)
{
}
#endif
