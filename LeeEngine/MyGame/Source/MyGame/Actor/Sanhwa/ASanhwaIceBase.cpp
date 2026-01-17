#include "CoreMinimal.h"
#include "ASanhwaIceBase.h"
#include "Engine/FAudioDevice.h"


AIceSpikeBase::AIceSpikeBase()
{
	SM_IceSpikes = std::make_shared<UStaticMeshComponent>();
	SM_IceSpikes->SetupAttachment(GetRootComponent());
	SM_IceSpikes->SetRelativeRotation(XMFLOAT3{90.0f,0.0f,0.0f});
	SM_IceSpikes->SetRelativeLocation(XMFLOAT3{0.0f,StartGroundDepth, 0.0f});

	SM_IceSpikes->SetCollisionObjectType(ECollisionChannel::Enemy);

	ExplosionAttackData = FAttackData{XMFLOAT3{500,500,500}, 3.0f, 0.0f, 5.0f, false};
}

void AIceSpikeBase::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache(GetIceSpikesStaticMeshName(),[this](std::shared_ptr<UObject> Object)
		{
			SM_IceSpikes->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			SM_IceSpikes->SetCollisionObjectType(ECollisionChannel::Enemy);
			SM_IceSpikes->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		});
	
}

void AIceSpikeBase::BeginPlay()
{
	AActor::BeginPlay();

	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Magic_Ice");

	XMFLOAT3 StartLocation = GetActorLocation();
	XMFLOAT3 EndLocation = StartLocation;
	EndLocation.y -= 1000.0f;
	std::vector<ECollisionChannel> CollisionChannel;
	for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
	{
		ECollisionChannel Cur = static_cast<ECollisionChannel>(i);
		if (Cur == ECollisionChannel::Enemy || Cur == ECollisionChannel::Player)
		{
			continue;
		}

		CollisionChannel.emplace_back(Cur);
	}
	FHitResult Result;
	
	if (GPhysicsEngine->LineTraceSingleByChannel(StartLocation,EndLocation, CollisionChannel, Result))
	{
		SetActorLocation(Result.Location);
	}

	GEngine->GetTimerManager()->SetTimer(SpawnTimerHandle, {this, &AIceSpikeBase::SpawnIce}, 0.0f, true, TimerTickTime);
	GEngine->GetTimerManager()->SetTimer(DestroyTimerHandle, Delegate<>{this, &AIceSpikeBase::DamagedByCharacter}, DestroyTime);
}

void AIceSpikeBase::OnDestroy()
{
	AActor::OnDestroy();

}

void AIceSpikeBase::SpawnIce()
{
	CurrentSpawnTime = std::min(CurrentSpawnTime + TimerTickTime, SpawnTime);

	SM_IceSpikes->SetRelativeLocation({0, std::lerp(StartGroundDepth, 0.0f, CurrentSpawnTime / SpawnTime), 0.0f});

	if (CurrentSpawnTime >= SpawnTime)
	{
		GEngine->GetTimerManager()->ClearTimer(SpawnTimerHandle);
	}
	
}

void AIceSpikeBase::SpawnedBy(AMyGameCharacterBase* Spawner)
{
	this->Spawner = Spawner;
}

float AIceSpikeBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	if (dynamic_cast<AMyGameCharacterBase*>(DamageCauser) == Spawner && DamageEvent.DamageType == "BreakIce")
	{
		bIsAttacked = true;
		DamagedByCharacter();
	}

	return DamageAmount;
}

void AIceSpikeBase::DamagedByCharacter()
{
	// TODO: 이펙트 같은거 꺼내기

	if (Spawner && bIsAttacked)
	{
		PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Explosion_Small");
		ExplosionAttackData.AttackCenterPos = GetActorLocation();
		Spawner->ApplyDamageToEnemy_Range(ExplosionAttackData);
	}

	GEngine->GetTimerManager()->ClearTimer(SpawnTimerHandle);
	GEngine->GetTimerManager()->ClearTimer(DestroyTimerHandle);

	DestroySelf();
}
