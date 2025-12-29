#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Physics/UBoxComponent.h"
#include "MyGame/Widget/Town/UEnchantWidget.h"


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

	Test = std::make_shared<UStaticMeshComponent>();
	Test->SetupAttachment(GetRootComponent());
	Test->SetRelativeScale3D(XMFLOAT3{10000.0f,10000.0f,10000.0f});
	Test->SetRelativeLocation({0.0f,100.0f,0.0f});

	Test2 = std::make_shared<UStaticMeshComponent>();
	Test2->SetupAttachment(GetRootComponent());
	Test2->SetRelativeLocation({0.0f,500.0f,0.0f});
}

void ATestCube2::Register()
{
	AActor::Register();

	/*AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Box",[this](std::shared_ptr<UObject> Object)
		{
			Mesh4->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Builing",[this](std::shared_ptr<UObject> Object)
		{
			Mesh0->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Mesh1->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Town_Hammer",[this](std::shared_ptr<UObject> Object)
		{
			Mesh1->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
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
		});*/

	AssetManager::GetAsyncAssetCache("SM_Water",[this](std::shared_ptr<UObject> Object)
		{
			Test->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Test->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_Dungeon",[this](std::shared_ptr<UObject> Object)
		{
			Test2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Test2->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});
	
}
void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();

}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		if (const std::shared_ptr<AActor>& Character = PC->GetPlayerCharacter())
		{
			float Distance =MyMath::GetDistance(Character->GetActorLocation(), GetActorLocation());
			if (Distance < 500.0f)
			{
				ActorComeToSmithy();
			}
			else
			{
				ActorComeOutSmithy();
			}
		}

	}
}

void ATestCube2::ActorComeToSmithy()
{
	if (bEnchantWidgetAttached)
	{
		return;
	}

	bEnchantWidgetAttached = true;

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		if (!EnchantWidget)
		{
			EnchantWidget = std::make_shared<UEnchantWidget>();
			PC->CreateWidget("EnchantWidget", EnchantWidget);
		}

		PC->AddToViewport("EnchantWidget",EnchantWidget);

		GEngine->SetInputMode(EInputMode::InputMode_UIOnly);
		GEngine->SetMouseLock(EMouseLockMode::DoNotLock);
		GEngine->ShowCursor(true);
	}
}

void ATestCube2::ActorComeOutSmithy()
{
	if (!bEnchantWidgetAttached)
	{
		return;
	}

	bEnchantWidgetAttached = false;
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		PC->RemoveFromParent("EnchantWidget");
		GEngine->SetInputMode(EInputMode::InputMode_GameOnly);
		GEngine->SetMouseLock(EMouseLockMode::LockAlways);
		GEngine->ShowCursor(false);
	}
}
