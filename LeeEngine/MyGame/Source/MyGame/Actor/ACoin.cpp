#include "CoreMinimal.h"
#include "ACoin.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Core/UMyGameInstance.h"
#include "MyGame/Widget/Town/UEquipmentStatusWidget.h"

ACoin::ACoin()
{
	Coin = std::make_shared<UStaticMeshComponent>();
	Coin->SetupAttachment(GetRootComponent());
	Coin->SetRelativeScale3D(XMFLOAT3{25,25,25});
}

void ACoin::Register()
{
	AActor::Register();

	Coin->GetBodyInstance()->SetSimulatePhysics(false);
	Coin->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Coin->SetCollisionObjectType(ECollisionChannel::WorldStatic);

	static std::shared_ptr<UStaticMesh> CoinMesh;
	if (!CoinMesh)
	{
		AssetManager::GetAsyncAssetCache("SM_Coin",[this](std::shared_ptr<UObject> Object)
			{
				CoinMesh = std::dynamic_pointer_cast<UStaticMesh>(Object);
				Coin->SetStaticMesh(CoinMesh);
			});	
	}
	else
	{
		
		Coin->SetStaticMesh(CoinMesh);
	}
}

void ACoin::OverlapToPlayer()
{
	if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
	{
		if (GameInstance->AddGold(10))
		{
			if (APlayerController* PC = GetWorld()->GetPlayerController())
			{
				if (const std::shared_ptr<UUserWidget>& Widget = PC->GetWidget("EquipmentStatus"))
				{
					if (const std::shared_ptr<UEquipmentStatusWidget>& StatusWidget = std::dynamic_pointer_cast<UEquipmentStatusWidget>(Widget))
					{
						StatusWidget->UpdateEquipmentData();
					}
				}
			}
		}
	}
	DestroySelf();
}

void ACoin::BeginPlay()
{
	AActor::BeginPlay();

}

void ACoin::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	constexpr float MoveStartTime = 1.5f;
	constexpr float LerpDuration = 0.75f;
	constexpr float TotalLifeTime = MoveStartTime + LerpDuration;

	Time = std::min(TotalLifeTime, Time + DeltaSeconds);

	if (RootComponent)
	{
		RootComponent->AddWorldRotation(XMFLOAT3{0.0f, 90.0f*DeltaSeconds, 0.0f});
	}

	if (bGoToPlayer)
	{
		if (const std::shared_ptr<AActor>& PlayerActor = GetWorld()->GetPlayerController()->GetPlayerCharacter())
		{
			SetActorLocation(MyMath::Lerp(OriginPos, PlayerActor->GetActorLocation(), (Time - MoveStartTime) / LerpDuration));
			if (Time >= TotalLifeTime)
			{
				OverlapToPlayer();
			}
		}
	}
	else
	{
		if (Time >= MoveStartTime)
		{
			bGoToPlayer = true;
			OriginPos = GetActorLocation();
		}
		
	}

}
