#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	{
		Ground= std::make_shared<UStaticMeshComponent>();
		Ground->SetupAttachment(GetRootComponent());
		Ground->SetRelativeScale3D({2500.0f,1.0f,2500.0f});	
		
	}

}

void ATestCube2::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();

}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
