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
	MY_GENERATED_BODY(ULevel)
public:
	ULevel();
	ULevel(const std::shared_ptr<UWorld>& World);
	~ULevel() override;
	void TickLevel(float DeltaSeconds);

	void SetOwningWorld(const std::shared_ptr<UWorld>& NewOwningWorld){ OwningWorld = NewOwningWorld; }
	std::shared_ptr<UWorld> GetWorld() const { return OwningWorld; }

	const std::vector<std::shared_ptr<AActor>>& GetLevelActors() const {return Actors;};

	void TestDrawLevel();

	void LoadDataFromFileData(std::map<std::string, std::string>& AssetData) override;
protected:
private:
public:
protected:
private:
	std::shared_ptr<UWorld>	OwningWorld;

	std::vector<std::shared_ptr<AActor>> Actors;

};
