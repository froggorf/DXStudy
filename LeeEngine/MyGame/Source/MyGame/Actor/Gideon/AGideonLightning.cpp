#include "CoreMinimal.h"
#include "AGideonLightning.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/World/UWorld.h"

AGideonLightning::AGideonLightning()
{
	LightningChainVFX = std::make_shared<UNiagaraComponent>();
	LightningChainVFX->SetupAttachment(GetRootComponent());
}

void AGideonLightning::Register()
{
	LightningChainVFX->SetNiagaraAsset(UNiagaraSystem::GetNiagaraAsset("NS_Lightning"));

	AActor::Register();
}

void AGideonLightning::BeginPlay()
{
	AActor::BeginPlay();
}

void AGideonLightning::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	// 잠시 나타나는 액터이므로 간단히 Tick에서 작업
	if (bVFXMoveStart)
	{
		float ChainCount = static_cast<float>(LightningChainPos.size());
		float MoveStep = DeltaSeconds * (1.0f / ChainToNextEnemyTime);
		float PrevMoveValue = VFXMoveValue;
		VFXMoveValue += MoveStep;

		// 마지막 구간까지 갔으면 종료
		if (VFXMoveValue >= ChainCount)
		{
			VFXMoveValue = ChainCount;
			bVFXMoveStart = false;
			return;
		}

		// 여러 구간을 한 번에 넘길 수도 있으므로 반복
		while (PrevMoveValue < VFXMoveValue)
		{
			int LastPosIndex = static_cast<int>(floor(PrevMoveValue));
			int NextPosIndex = LastPosIndex + 1;

			if (NextPosIndex >= LightningChainPos.size())
				break;

			float LerpValue;
			if (VFXMoveValue < static_cast<float>(NextPosIndex))
			{
				// 현재 구간 진행 중
				LerpValue = VFXMoveValue - static_cast<float>(LastPosIndex);
			}
			else
			{
				// 구간을 넘어섰으므로 끝까지 이동
				LerpValue = 1.0f;
			}

			const XMFLOAT3& Last = LightningChainPos[LastPosIndex];
			const XMFLOAT3& Next = LightningChainPos[NextPosIndex];

			LightningChainVFX->SetWorldLocation(MyMath::Lerp(Last, Next, LerpValue));

			PrevMoveValue = static_cast<float>(NextPosIndex);
		}
	}
}

void AGideonLightning::Initialize(AGideonCharacter* DamageCauser, const FAttackData& Damage, UINT ApplyCount, UINT ChargeLevel)
{
	GideonCharacter = DamageCauser;
	this->ApplyCount = ApplyCount;
	TargetEnemies.reserve(ApplyCount);
	AttackData = Damage;
	this->ChargeLevel = ChargeLevel;
	LightningChainPos.clear();
	LightningChainPos.emplace_back(GetActorLocation());
	FindEnemy();
}

void AGideonLightning::FindEnemy()
{
	std::vector<AActor*> NextChainEnemies;

	const XMFLOAT3& RecentChainPos = GetRecentChainPos();
	// 기존의 TargetEnemies를 ActorsToIgnore 로 건네주어서 중복 체크를 할 필요가 없음
	GPhysicsEngine->SphereOverlapComponents(RecentChainPos, ChainMaxDistance, {ECollisionChannel::Enemy}, 
											TargetEnemies, NextChainEnemies);

	if (!NextChainEnemies.empty())
	{
		float MinDistance = FLT_MAX;
		AActor* MinDistanceEnemy = nullptr;
		for (AActor* Enemy : NextChainEnemies)
		{
			float CurDistance = MyMath::GetDistance(Enemy->GetActorLocation(), RecentChainPos);
			if (CurDistance < MinDistance)
			{
				MinDistance = CurDistance;
				MinDistanceEnemy = Enemy;
			}
		}

		if (MinDistanceEnemy)
		{
			TargetEnemies.emplace_back(MinDistanceEnemy);
			LightningChainPos.emplace_back(MinDistanceEnemy->GetActorLocation());
		}
	}

	--ApplyCount;
	if (ApplyCount > 0)
	{
		FindEnemy();
	}
	else
	{
		StartAttack();
	}
}

void AGideonLightning::DestroySelf()
{
	AActor::DestroySelf();
}

void AGideonLightning::StartAttack()
{
	for (AActor* Enemy : TargetEnemies)
	{
		for (UINT i = 0; i < ChargeLevel; ++i)
		{
			GideonCharacter->ApplyDamageToEnemy(Enemy, AttackData, "GideonHeavyAttack");		
		}
	}

	bVFXMoveStart = true;

	GEngine->GetTimerManager()->SetTimer(KillSelfTimerHandle, {this, &AGideonLightning::DestroySelf}, KillSelfTime);
}

XMFLOAT3 AGideonLightning::GetRecentChainPos()
{
	return LightningChainPos.empty() ? GetActorLocation() : *LightningChainPos.rbegin();
}
