#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	if (!GDirectXDevice) return;

	{
		SM_Chair = std::make_shared<UStaticMeshComponent>();
		SM_Chair->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Chair",[this](std::shared_ptr<UObject> Object)
			{
				SM_Chair->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Chair->SetRelativeLocation({-300,0,0});
	}

	{
		SM_Couch = std::make_shared<UStaticMeshComponent>();
		SM_Couch->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_UE_Couch",[this](std::shared_ptr<UObject> Object)
			{
				SM_Couch->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		SM_Couch->SetRelativeLocation({-300,0,-250});
	}

	{
		Ground= std::make_shared<UStaticMeshComponent>();
		Ground->SetupAttachment(GetRootComponent());
		AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
			{
				Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			});
		Ground->SetRelativeLocation({0,0,0});
		Ground->SetRelativeScale3D({5000,10,5000});
	}

	

	
}

void ATestCube2::Register()
{
	AActor::Register();

	
	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();
	
	Ground->SetCollisionResponseToChannel(ECollisionChannel::Pawn, ECollisionResponse::Block);
	
	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
