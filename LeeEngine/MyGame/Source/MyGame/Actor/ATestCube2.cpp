#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	{
		Ground= std::make_shared<UStaticMeshComponent>();
		Ground->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
			{
				Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		Ground->SetRelativeLocation({0,-10,0});
		Ground->SetRelativeScale3D({5000,20,5000});
	}

	{
		Ground2= std::make_shared<UStaticMeshComponent>();
		Ground2->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
			{
				Ground2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		Ground2->SetRelativeLocation({0,-10,0});
		Ground2->SetRelativeScale3D({5000,20,5000});
	}

	{
		SM_Chair = std::make_shared<UStaticMeshComponent>();
		SM_Chair->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Chair",[this](std::shared_ptr<UObject> Object)
			{
				SM_Chair->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Chair->SetRelativeLocation({-300,10,0});
	}

	{
		SM_Couch = std::make_shared<UStaticMeshComponent>();
		SM_Couch->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Couch",[this](std::shared_ptr<UObject> Object)
			{
				SM_Couch->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Couch->SetRelativeLocation({-300,10,-250});
		SM_Couch->SetIsMonochromeObject(false);
	}
}

void ATestCube2::Register()
{
	AActor::Register();

}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();

}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
