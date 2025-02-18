// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "AActor.h"

#include "Engine/Components/USceneComponent.h"

unsigned int ActorIDCount = 0;

AActor::AActor()
{
	RootComponent = std::make_shared<USceneComponent>();
	
	ActorID = ActorIDCount++;
	Rename("Actor_" + std::to_string(ActorID));
}

XMFLOAT3 AActor::GetActorLocation() const
{
	XMFLOAT3 RetLoc = XMFLOAT3{0.0f,0.0f,0.0f};
	if(RootComponent)
	{
		RetLoc = RootComponent->GetRelativeLocation();
	}
	return RetLoc;
}

void AActor::SetActorLocation(const XMFLOAT3& NewLocation) const
{
	if(RootComponent)
	{
		// TODO: 언리얼엔진에서는 MoveComponent를 통한 이동을 진행
		RootComponent->SetRelativeLocation(NewLocation);
	}
}

void AActor::Tick(float DeltaSeconds)
{
}

void AActor::TestDraw()
{
	if(std::shared_ptr<USceneComponent> RootComponent  = GetRootComponent())
	{
		RootComponent->TestDraw();
	}
	
}
