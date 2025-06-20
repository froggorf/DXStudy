// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "ULevel.h"

#include "Engine/GameFramework/AActor.h"

#include "Engine/UEditorEngine.h"
#include "Engine/RenderCore/EditorScene.h"

static int LevelCount = 0;

ULevel::ULevel()
{
	Rename("Level" + LevelCount++);
}

ULevel::ULevel(const ULevel* LevelInstance)
{
	OwningWorld = GEngine->GetWorld();
	Rename(LevelInstance->GetName());

	nlohmann::basic_json<> ActorsData = LevelInstance->LevelData["Actor"];
	size_t ActorCount = ActorsData.size();
	Actors.reserve(ActorCount);
	for (size_t i = 0; i < ActorCount; ++i)
	{
		nlohmann::basic_json<>  ActorData = ActorsData[i];
		std::string             ClassName = ActorData["Class"];
		std::shared_ptr<AActor> NewActor  = std::dynamic_pointer_cast<AActor>(GetDefaultObject(ClassName)->CreateInstance());
		if (NewActor)
		{
			NewActor->LoadDataFromFileData(ActorData);
			Actors.push_back(NewActor);
		}
	}
	/*size_t      ActorCount  = LevelInstance->GetLevelActors().size();
	const auto& LevelActors = LevelInstance->GetLevelActors();
	Actors.reserve(ActorCount);
	for (const auto& Actor : LevelActors)
	{
		std::shared_ptr<AActor> NewActor = std::dynamic_pointer_cast<AActor>(Actor->CreateInstance());
		NewActor->Rename(Actor->GetName());
		NewActor->SetActorLocation(Actor->GetActorLocation());
		NewActor->SetActorRotation(Actor->GetActorRotation());
		NewActor->SetActorScale3D(Actor->GetActorScale3D());
		Actors.push_back(NewActor);
	}*/
}

//ULevel::ULevel(const std::shared_ptr<UWorld>& World)
//{
//	OwningWorld = World;
//
//	std::shared_ptr<AActor> TestActor = std::make_shared<ATestCube>();
//	Actors.push_back(TestActor);
//
//}

ULevel::~ULevel()
{
}

void ULevel::PostLoad()
{
	UObject::PostLoad();
}

void ULevel::Register()
{
	UObject::Register();

	// ImGUI 등을 포함한 EditorScene을 추가하기 위해
	// WITH_EDITOR 을 분기점으로 적용
#ifdef WITH_EDITOR
	FEditorScene::InitSceneData_GameThread();
#else
	FScene::InitSceneData_GameThread();
#endif

	for (const auto& Actor : Actors)
	{
		Actor->Register();
	}
}

void ULevel::BeginPlay()
{
	UObject::BeginPlay();

	for (const auto& Actor : Actors)
	{
		Actor->BeginPlay();
	}
}

void ULevel::TickLevel(float DeltaSeconds)
{
	for (const auto& Actor : Actors)
	{
		Actor->Tick(DeltaSeconds);
	}
}

void ULevel::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	std::string LevelName = AssetData["Name"];

	UObject::LoadDataFromFileData(AssetData);

	LevelData = AssetData;
}

void ULevel::SaveDataFromAssetToFile(nlohmann::json& Json)
{
	UObject::SaveDataFromAssetToFile(Json);

	for (const auto& Actor : Actors)
	{
		nlohmann::json ActorJson;
		Actor->SaveDataFromAssetToFile(ActorJson);

		Json["Actor"].push_back(ActorJson);
	}
}
