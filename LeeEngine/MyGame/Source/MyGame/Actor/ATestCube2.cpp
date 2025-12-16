#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	constexpr float Size = 0.2f;

	Ground = std::make_shared<UStaticMeshComponent>();
	Ground->SetupAttachment(GetRootComponent());
	Ground->SetRelativeScale3D({5000.0f, 5.0f, 5000.0f});
	Ground->SetRelativeLocation({0.0f, -100.0f,0.0f});

	Mesh0 = std::make_shared<UStaticMeshComponent>();
	Mesh0 ->SetupAttachment(GetRootComponent());
	Mesh0 ->SetRelativeScale3D({Size, Size, Size});

	Mesh1= std::make_shared<UStaticMeshComponent>();
	Mesh1->SetupAttachment(GetRootComponent());
	Mesh1->SetRelativeScale3D({Size, Size, Size});

	Mesh2= std::make_shared<UStaticMeshComponent>();
	Mesh2->SetupAttachment(GetRootComponent());
	Mesh2->SetRelativeScale3D({Size, Size, Size});

	Mesh3= std::make_shared<UStaticMeshComponent>();
	Mesh3->SetupAttachment(GetRootComponent());
	Mesh3->SetRelativeScale3D({Size, Size, Size});

	Mesh4= std::make_shared<UStaticMeshComponent>();
	Mesh4->SetupAttachment(GetRootComponent());
	Mesh4->SetRelativeScale3D({Size, Size, Size});

}

void ATestCube2::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Box",[this](std::shared_ptr<UObject> Object)
		{
			Mesh4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh4->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Builing",[this](std::shared_ptr<UObject> Object)
		{
			Mesh0->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh1->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Hammer",[this](std::shared_ptr<UObject> Object)
		{
			Mesh1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh1->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Roof1",[this](std::shared_ptr<UObject> Object)
		{
			Mesh2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh2->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Roof2",[this](std::shared_ptr<UObject> Object)
		{
			Mesh3->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh3->SetCollisionObjectType(ECollisionChannel::WorldStatic);
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
