#include "CoreMinimal.h"
#include "AGideonIceStorm.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/World/UWorld.h"
#include "Engine/FAudioDevice.h"

AGideonIceStorm::AGideonIceStorm()
{
	IceStormVFX = std::make_shared<UNiagaraComponent>();
	IceStormVFX->SetupAttachment(GetRootComponent());
	IceStormVFX->SetRelativeLocation({0.0f,0.0f,0.0f});
	IceStormVFX->SetRelativeScale3D({0.0f,0.0f,0.f});

	StormCollisionComponent = std::make_shared<UStaticMeshComponent>();
	StormCollisionComponent->SetupAttachment(GetRootComponent());
	StormCollisionComponent->SetRelativeScale3D({20,20,20});


	if (!GDirectXDevice)
	{
		return;
	}

	static const std::shared_ptr<UNiagaraSystem>& IceStormNiagara = UNiagaraSystem::GetNiagaraAsset("NS_IceStorm");
	IceStormVFX->SetNiagaraAsset(IceStormNiagara);
}

void AGideonIceStorm::Register()
{
	// TODO: 나이아가라 시스템 만들고서 등록하기
	//LightningChainVFX->SetNiagaraAsset(UNiagaraSystem::GetNiagaraAsset("NS_Lightning"));
	AssetManager::GetAsyncAssetCache("SM_Storm",[this](std::shared_ptr<UObject> Object)
		{
			StormCollisionComponent->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			StormCollisionComponent->SetCollisionEnabled(ECollisionEnabled::Physics);

			//StormCollisionComponent->SetVisibility(false);
		});

	AActor::Register();

}

void AGideonIceStorm::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	XMFLOAT3 ForwardXZ = GetActorForwardVector();
	ForwardXZ.y = 0.0f;
	ForwardXZ = ForwardXZ * MoveSpeedPerSec * DeltaSeconds;
	SetActorLocation(GetActorLocation() + ForwardXZ);

	StormSoundTimer -= DeltaSeconds;
	if (StormSoundTimer <= 0.0f)
	{
		PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Strom");
		StormSoundTimer = StormSoundInterval;
	}
}

void AGideonIceStorm::Initialize(AGideonCharacter* DamageCauser, const FAttackData& Damage)
{
	GideonCharacter = DamageCauser;
	
	AttackData = Damage;

	GEngine->GetTimerManager()->SetTimer(KillSelfTimerHandle, {this, &AGideonIceStorm::SetLifeTime_Timer}, 0.0f, true, SetScaleTimerTickTime);
	GEngine->GetTimerManager()->SetTimer(FindEnemyAndApplyDamageTimerHandle, {this, &AGideonIceStorm::FindEnemy}, 0.0f, true, FindEnemyAndApplyDamageTickTime);
}

void AGideonIceStorm::FindEnemy()
{
	std::vector<AActor*> OverlapEnemies;

	GPhysicsEngine->SphereOverlapComponents(GetActorLocation(), 1000.0f, {ECollisionChannel::Enemy}, {}, OverlapEnemies);
	if (!OverlapEnemies.empty())
	{
		for (AActor* Enemy : OverlapEnemies)
		{
			float CurDistance = MyMath::GetDistance(Enemy->GetActorLocation(), GetActorLocation());
			if (CurDistance <= ApplyDamageDistance)
			{
				GideonCharacter->ApplyDamageToEnemy(Enemy, AttackData, "G_Skill");
			}
		}

		// 무조건 0번째 적에게 대해서 접근하도록 설정
		
		XMFLOAT4 ToEnemyRotation = MyMath::GetRotationQuaternionToActor(GetActorLocation(), OverlapEnemies[0]->GetActorLocation());
		SetActorRotation(ToEnemyRotation);
	}

}

void AGideonIceStorm::SetLifeTime_Timer()
{
	static constexpr float SetScaleTime = 0.5f;
	CurrentLifeTime += SetScaleTimerTickTime;
	if (CurrentLifeTime <= SetScaleTime)
	{
		float NewScale = std::clamp(std::lerp(0.0f, 1.0f, 1 - (SetScaleTime - CurrentLifeTime) / SetScaleTime), 0.0f, 1.0f);
		IceStormVFX->SetRelativeScale3D(XMFLOAT3{NewScale,NewScale,NewScale});
	}
	else if (CurrentLifeTime >= KillSelfTime - SetScaleTime)
	{
		float NewScale = std::lerp(0.0f,1.0f, (KillSelfTime - CurrentLifeTime) / SetScaleTime);
		NewScale = std::clamp(NewScale, 0.0f,1.0f);
		IceStormVFX->SetRelativeScale3D(XMFLOAT3{NewScale,NewScale,NewScale});
	}

	if (CurrentLifeTime >= KillSelfTime)
	{
		GEngine->GetTimerManager()->ClearTimer(KillSelfTimerHandle);
		GEngine->GetTimerManager()->ClearTimer(FindEnemyAndApplyDamageTimerHandle);
		
		DestroySelf();
	}
}
