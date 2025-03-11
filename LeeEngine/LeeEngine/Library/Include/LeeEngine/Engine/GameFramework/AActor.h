// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <memory>

#include "Engine/UObject/UObject.h"

class USceneComponent;

class AActor : public UObject, public std::enable_shared_from_this<AActor>
{
	MY_GENERATED_BODY(AActor)

public:
	AActor();
	
	

	virtual void Init() override;
	virtual void Register() override;
	const std::shared_ptr<USceneComponent>& GetRootComponent() const { return RootComponent; }

	XMFLOAT3 GetActorLocation() const;
	XMFLOAT4 GetActorRotation() const;
	XMFLOAT3 GetActorScale3D() const;
	void SetActorLocation(const XMFLOAT3& NewLocation) const;
	void SetActorRotation(const XMFLOAT4& NewRotation) const;
	void SetActorScale3D(const XMFLOAT3& NewScale3D) const;

	virtual void Tick(float DeltaSeconds);
protected:

private:

public:

protected:
	// 월드 내에서 Transform (Loc, Rot, Scale) 의 정보를 다루는 컴퍼넌트
	// 모든 다른 컴퍼넌트는 해당 컴퍼넌트에 부착해야함
	std::shared_ptr<USceneComponent>		RootComponent;

protected:
	unsigned int ActorID = 0;
	
	
};
