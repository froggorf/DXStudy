// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "ULevel.h"

#include "../../../content/Actor/ATestCube.h"
#include "Engine/GameFramework/AActor.h"

#include <fstream>

#include "nlohmann/json.hpp"

static int LevelCount = 0;

ULevel::ULevel()
{
	Rename("Level"+ LevelCount++);
}

ULevel::ULevel(const std::shared_ptr<UWorld>& World)
{
	OwningWorld = World;

	std::shared_ptr<AActor> TestActor = std::make_shared<ATestCube>();
	Actors.push_back(TestActor);

}

ULevel::~ULevel()
{
}

void ULevel::TickLevel(float DeltaSeconds)
{

	for(const auto& Actor : Actors)
	{
		Actor->Tick(DeltaSeconds);
	}
}

void ULevel::TestDrawLevel()
{
	for(const auto& Actor : Actors)
	{
		Actor->TestDraw();
	}
}

void ULevel::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	auto Actors = AssetData["Actor"];
	int ActorCount = Actors.size();
	for(int i = 0; i < ActorCount; ++i)
	{
		
	}
}

