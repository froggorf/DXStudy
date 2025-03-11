// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "Engine/UObject/UObject.h"
#include "AActor.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"

unsigned int ActorIDCount = 0;

//AActor::MakeCDO_ AActor::_initializer;

AActor::AActor()
{
	AActor::Init();
}

void AActor::Init()
{
	UObject::Init();
	RootComponent = std::make_shared<USceneComponent>();
	RootComponent->Rename("RootComponent");

	ActorID = ActorIDCount++;
	Rename("Actor_" + std::to_string(ActorID));
}

void AActor::Register()
{
	UObject::Register();

	RootComponent->Register();

	FScene::AddWorldOutlinerActor_GameThread(shared_from_this());
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

XMFLOAT4 AActor::GetActorRotation() const
{
	XMFLOAT4 RetRotQuat = XMFLOAT4(0.0f,0.0f,0.0f,1.0f);
	if(RootComponent)
	{
		RetRotQuat = RootComponent->GetRelativeRotation();
	}
	return RetRotQuat;
}

XMFLOAT3 AActor::GetActorScale3D() const
{
	XMFLOAT3 RetScale = XMFLOAT3(1.0f,1.0f,1.0f);
	if(RootComponent)
	{
		RetScale = RootComponent->GetRelativeScale3D();
	}
	return RetScale;
}

void AActor::SetActorLocation(const XMFLOAT3& NewLocation) const
{
	if(RootComponent)
	{
		// TODO: 언리얼엔진에서는 MoveComponent를 통한 이동을 진행
		RootComponent->SetRelativeLocation(NewLocation);
	}
}

void AActor::SetActorRotation(const XMFLOAT4& NewRotation) const
{
	if(RootComponent)
	{
		// TODO: 언리얼엔진에서는 MoveComponent를 통한 이동을 진행
		RootComponent->SetRelativeRotation(NewRotation);
	}
}

void AActor::SetActorScale3D(const XMFLOAT3& NewScale3D) const
{
	if(RootComponent)
	{
		// TODO: 언리얼엔진에서는 MoveComponent를 통한 이동을 진행
		RootComponent->SetRelativeScale3D(NewScale3D);
	}
}

void AActor::Tick(float DeltaSeconds)
{
}

