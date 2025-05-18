// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"

class AActor;

class UActorComponent : public UObject
{
	MY_GENERATED_BODY(UActorComponent)
	UActorComponent();

	virtual void UpdateComponentToWorld()
	{
	}

	virtual void TickComponent(float DeltaSeconds);

	AActor* GetOwner() const
	{
		return OwnerPrivate;
	}

	void SetOwner(AActor* InOwner)
	{
		OwnerPrivate = InOwner;
	}

#ifdef WITH_EDITOR
	virtual void DrawDetailPanel(UINT ComponentDepth);
#endif

public:
	UINT ComponentID;

private:
	AActor* OwnerPrivate;
};
