// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


// VFX 업데이트, Subsystem업데이트, 레벨 업데이트 같은 전체적인 역할을 관리하는 클래스
#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/Level/ULevel.h"
#include "Engine/UObject/UObject.h"

class ULevel;

class UWorld : public UObject
{
public:
	UWorld();

	void Init() override;
	void PostLoad() override;

	void TickWorld(float DeltaSeconds);
	void Tick();


	std::shared_ptr<ULevel> GetPersistentLevel() const { return PersistentLevel; }
	void SetPersistentLevel(ULevel* NewLevel) { PersistentLevel = std::make_shared<ULevel>(*NewLevel); }

	void TestDrawWorld();
	
protected:
private:
public:
protected:
private:
	// 콜렉션과 관련된 현재 레벨
	std::shared_ptr<ULevel>	PersistentLevel;

	// 콜렉션에 있는 모든 레벨정보
	std::set<std::shared_ptr<ULevel>> Levels;
};
