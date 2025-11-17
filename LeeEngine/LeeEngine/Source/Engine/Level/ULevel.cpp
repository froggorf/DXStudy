// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "ULevel.h"

#include "Engine/GameFramework/AActor.h"
#include "Engine/World/UWorld.h"

static int LevelCount = 0;

ULevel::ULevel()
{
	Rename("Level" + LevelCount++);
}

ULevel::ULevel(const ULevel* LevelInstance)
{
	OwningWorld = GEngine->GetCurrentWorld();
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
}


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

	for (size_t Index = 0; Index < Actors.size(); ++Index)
	{
		Actors[Index]->BeginPlay();
	}
}

void ULevel::TickLevel(float DeltaSeconds)
{
	for (const std::shared_ptr<AActor>& PendingAddActor : PendingAddActors)
	{
		Actors.emplace_back(PendingAddActor);
		PendingAddActor->BeginPlay();
	}
	PendingAddActors.clear();


	for (const auto& Actor : Actors)
	{
		Actor->Tick(DeltaSeconds * Actor->GetTickRate());
	}
}

void ULevel::Tick_Editor(float DeltaSeconds)
{
	for (const auto& Actor : Actors)
	{
		Actor->Tick_Editor(DeltaSeconds);
	}
}

void ULevel::DestroyActor(AActor* RemoveActor)
{
	for (std::shared_ptr<AActor>& Actor : Actors)
	{
		if (Actor.get() == RemoveActor)
		{
			PendingKillActors.emplace_back(Actor);
			Actor->NotePendingKill();
		}
	}
}

void ULevel::UnregisterPendingActors()
{
	for (std::shared_ptr<AActor>& PendingKillActor : PendingKillActors)
	{
		PendingKillActor->OnDestroy();
		PendingKillActor->UnRegister();
	}
	PendingKillActors.clear();

	auto PendingKillIter = std::remove_if(Actors.begin(),Actors.end(), [](const std::shared_ptr<AActor>& Actor){return Actor->GetPendingKill();});
	Actors.erase(PendingKillIter ,Actors.end());
}

std::shared_ptr<AActor> ULevel::FindSharedActorByRawPointer(AActor* Actor)
{
	for (size_t i = 0; i < Actors.size(); ++i)
	{
		if (Actor == Actors[i].get())
		{
			return Actors[i];
		}
	}
	return nullptr;
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

std::shared_ptr<AActor> ULevel::SpawnActor(const std::string& ClassName, const FTransform& SpawnTransform)
{
	std::unordered_map<std::string, std::shared_ptr<UObject>>& CDOMap = UObject::GetCDOMap();
	auto P = CDOMap.find(ClassName);
	if (P == CDOMap.end())
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "Invalid class name");
		return nullptr;
	}

	std::shared_ptr<AActor> NewActor  = std::dynamic_pointer_cast<AActor>(P->second->CreateInstance());
	if (NewActor)
	{
		NewActor->SetActorLocation(SpawnTransform.GetTranslation());
		NewActor->SetActorRotation(SpawnTransform.GetRotation());
		NewActor->SetActorScale3D(SpawnTransform.GetScale3D());
		
		PendingAddActors.emplace_back(NewActor);
		NewActor->Register();
	}

	return NewActor;
}
