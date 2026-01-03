#include "CoreMinimal.h"
#include "ADungeon.h"

ADungeon::ADungeon()
{
	Water = std::make_shared<UStaticMeshComponent>();
	Water->SetupAttachment(GetRootComponent());
	Water->SetRelativeScale3D(XMFLOAT3{100000.0f,100000.0f,100000.0f});
	Water->SetRelativeLocation({0.0f,0.0f,0.0f});

	Ground = std::make_shared<UStaticMeshComponent>();
	Ground->SetupAttachment(GetRootComponent());
	Ground->SetRelativeLocation({0.0f,0.0f,0.0f});
}

void ADungeon::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Water",[this](std::shared_ptr<UObject> Object)
		{
			Water->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Water->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Dungeon",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

}

