#include "CoreMinimal.h"
#include "ACoin.h"

#include "Engine/World/UWorld.h"

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
	//TODO: 여기서 골드 획득 처리하기
	DestroySelf();
}

void ACoin::BeginPlay()
{
	AActor::BeginPlay();

}

void ACoin::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	Time = std::min(3.0f, Time + DeltaSeconds);

	if (RootComponent)
	{
		RootComponent->AddWorldRotation(XMFLOAT3{0.0f, 90.0f*DeltaSeconds, 0.0f});
	}

	if (bGoToPlayer)
	{
		if (const std::shared_ptr<AActor>& PlayerActor = GetWorld()->GetPlayerController()->GetPlayerCharacter())
		{
			SetActorLocation(MyMath::Lerp(OriginPos, PlayerActor->GetActorLocation(), (Time - 1.5f)/1.5f));
			if (Time >= 3.0f)
			{
				OverlapToPlayer();
			}
		}
	}
	else
	{
		if (Time >= 1.5f)
		{
			bGoToPlayer = true;
			OriginPos = GetActorLocation();
		}
		
	}

}
