// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"
#include "Engine/MyEngineUtils.h"
class AGameMode;
class AActor;
class UWorld;

class ULevel : public UObject
{
	MY_GENERATE_BODY(ULevel)
	

	ULevel();

	// 레벨 정보를 생성자를 통해서 인스턴스를 만드는 방식
	ULevel(const ULevel* LevelInstance);
	~ULevel() override;

	void PostLoad() override;
	void Register() override;
	void BeginPlay() override;
	void TickLevel(float DeltaSeconds);
	void Tick_Editor(float DeltaSeconds);

	void SetOwningWorld(const std::shared_ptr<UWorld>& NewOwningWorld)
	{
		OwningWorld = NewOwningWorld;
	}

	std::shared_ptr<UWorld> GetWorld() const
	{
		return OwningWorld;
	}

	const std::vector<std::shared_ptr<AActor>>& GetLevelActors() const
	{
		return Actors;
	};


	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	void SaveDataFromAssetToFile(nlohmann::json& Json) override;

	std::shared_ptr<AActor> SpawnActor(const std::string& ClassName, const FTransform& SpawnTransform, const std::string& Name = "");
	void DestroyActor(AActor* RemoveActor);
	void UnregisterPendingActors();

	const std::shared_ptr<AActor>& FindActorByName(const std::string& Name);
 	std::shared_ptr<AActor> FindSharedActorByRawPointer(AActor* Actor);

	std::shared_ptr<AGameMode> GetGameMode() const {return GameModeActor;}
private:

	std::shared_ptr<UWorld> OwningWorld;

	std::vector<std::shared_ptr<AActor>> Actors;
	std::vector<std::shared_ptr<AActor>> PendingAddActors;
	std::vector<std::shared_ptr<AActor>> PendingKillActors;

	std::shared_ptr<AGameMode> GameModeActor;
private:
	nlohmann::basic_json<> LevelData;
};
