// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/World/UWorld.h"
class AActor;
class UWorld;

class ULevel : public UObject
{
public:
	ULevel();

	void TickLevel(float DeltaSeconds);

	void SetOwningWorld(UWorld* NewOwningWorld){ OwningWorld = std::make_shared<UWorld>(*NewOwningWorld); }
	std::shared_ptr<UWorld> GetWorld() const { return OwningWorld; }


	void TestDrawLevel();
protected:
private:
public:
protected:
private:
	std::shared_ptr<UWorld>	OwningWorld;

	std::vector<std::shared_ptr<AActor>> Actors;
};
