// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <memory>

#include "Engine/UObject/UObject.h"

class USceneComponent;

class AActor : public UObject
{
	MY_GENERATED_BODY(AActor)

public:
	AActor();
	
	

	virtual void Init() override;

	const std::shared_ptr<USceneComponent>& GetRootComponent() const { return RootComponent; }

	XMFLOAT3 GetActorLocation() const;
	void SetActorLocation(const XMFLOAT3& NewLocation) const;
	virtual void Tick(float DeltaSeconds);
	virtual void TestDraw();
protected:

private:

public:

protected:
	// 월드 내에서 Transform (Loc, Rot, Scale) 의 정보를 다루는 컴퍼넌트
	// 모든 다른 컴퍼넌트는 해당 컴퍼넌트에 부착해야함
	std::shared_ptr<USceneComponent>		RootComponent;

private:
	unsigned int ActorID = 0;
	
	
};
