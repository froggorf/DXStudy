#include "CoreMinimal.h"
#include "AEnemySpawner.h"

#include "Engine/Components/UStaticMeshComponent.h"

AEnemySpawner::AEnemySpawner()
{
	Arrow = std::make_shared<UStaticMeshComponent>();
	Arrow->SetupAttachment(GetRootComponent());
	Arrow->SetRelativeRotation(XMFLOAT3{0,-90,0});
	Arrow->SetRelativeScale3D(XMFLOAT3{15,15,15});
}

void AEnemySpawner::Register()
{
	AActor::Register();

#ifdef WITH_EDITOR
	AssetManager::GetAsyncAssetCache("SM_Arrow", [this](std::shared_ptr<UObject> Object)
		{
			Arrow->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
		});
#endif
}

