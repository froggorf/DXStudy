#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{

	TestComp = std::make_shared<UStaticMeshComponent>();
	AssetManager::GetAsyncAssetCache("SM_Box",[this](std::shared_ptr<UObject> Object)
		{
			TestComp->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	//TestComp->SetupAttachment(GetRootComponent());
	//TestComp->SetRelativeScale3D({30,30,30});

	//TestComp->SetCollisionObjectType(ECollisionChannel::WorldDynamic);
	
	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();
	//TestComp->SetCollisionEnabled(ECollisionEnabled::Physics);
	//TestComp->GetBodyInstance()->SetCollisionResponseToChannel(ECollisionChannel::WorldStatic,ECollisionResponse::Block);
	//TestComp.setcollision
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
