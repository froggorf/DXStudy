// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "ULevel.h"

#include "Engine/GameFramework/AActor.h"

#include <fstream>

#include "Engine/UEditorEngine.h"
#include "Engine/RenderCore/EditorScene.h"
#include "nlohmann/json.hpp"

static int LevelCount = 0;

ULevel::ULevel()
{
	Rename("Level"+ LevelCount++);
}

ULevel::ULevel(const ULevel* LevelInstance)
{
	OwningWorld = GEngine->GetWorld();

	int ActorCount = LevelInstance->GetLevelActors().size();
	const auto& LevelActors = LevelInstance->GetLevelActors();
	Actors.reserve(ActorCount);
	for(const auto& Actor : LevelActors)
	{
		std::shared_ptr<AActor> NewActor = std::dynamic_pointer_cast<AActor>(Actor->CreateInstance());
		NewActor->Rename(Actor->GetName());
		NewActor->SetActorLocation(Actor->GetActorLocation());
		NewActor->SetActorRotation(Actor->GetActorRotation());
		NewActor->SetActorScale3D(Actor->GetActorScale3D());
		Actors.push_back(NewActor);
		
	}
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

	FScene::InitSceneData_GameThread();
	for(const auto& Actor : Actors)
	{
		Actor->Register();
	}
}

void ULevel::TickLevel(float DeltaSeconds)
{

	for(const auto& Actor : Actors)
	{
		Actor->Tick(DeltaSeconds);
	}
}


void ULevel::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	std::string LevelName = AssetData["Name"];
	if(GetLevelInstanceMap().contains(LevelName))
	{
		MY_LOG("LoadLevel",EDebugLogLevel::DLL_Warning, "Already exist level");
		return;
	}

	UObject::LoadDataFromFileData(AssetData);

	auto ActorsData = AssetData["Actor"];
	int ActorCount = ActorsData.size();
	Actors.reserve(ActorCount);
	for(int i = 0; i < ActorCount; ++i)
	{
		auto ActorData = ActorsData[i];
		std::string ClassName = ActorData["Class"];
		std::shared_ptr<AActor> NewActor = std::dynamic_pointer_cast<AActor>(GetDefaultObject(ClassName)->CreateInstance());
		if(NewActor)
		{
			NewActor->Rename(ActorData["Name"]);
			int X = 0, Y = 1, Z = 2, W =3;
			auto LocationData = ActorData["Location"];
			NewActor->SetActorLocation(XMFLOAT3(LocationData[X],LocationData[Y],LocationData[Z]));
			auto RotationData = ActorData["Rotation"];
			NewActor->SetActorRotation(XMFLOAT4(RotationData[X],RotationData[Y],RotationData[Z],RotationData[W]));
			auto ScaleData = ActorData["Scale"];
			NewActor->SetActorScale3D(XMFLOAT3(ScaleData[X],ScaleData[Y],ScaleData[Z]));
			Actors.push_back(NewActor);
		}
	}

	GetLevelInstanceMap()[LevelName] = std::make_unique<ULevel>(*this);
}

