#include "CoreMinimal.h"
#include "AGideonMeteor.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/World/UWorld.h"

AGideonMeteor::AGideonMeteor()
{
	MeteorVFX = std::make_shared<UNiagaraComponent>();
	MeteorVFX->SetupAttachment(GetRootComponent());

}

void AGideonMeteor::Register()
{
	static const std::shared_ptr<UNiagaraSystem>& NS_Meteor = UNiagaraSystem::GetNiagaraAsset("NS_Meteor");
	MeteorVFX->SetNiagaraAsset(NS_Meteor);

	AActor::Register();
}

void AGideonMeteor::BeginPlay()
{
	AActor::BeginPlay();
	GEngine->GetTimerManager()->SetTimer(SelfKillTimerHandle, {this, &AGideonMeteor::DestroySelf}, SelfKillTime);
	GEngine->GetTimerManager()->SetTimer(MoveTimerHandle, {this, &AGideonMeteor::MoveTick}, 0.0f, true, MoveTickTime);
}

void AGideonMeteor::OnDestroy()
{
	AActor::OnDestroy();

	GEngine->GetTimerManager()->ClearTimer(SelfKillTimerHandle);	
	GEngine->GetTimerManager()->ClearTimer(MoveTimerHandle);	
}

void AGideonMeteor::DestroySelf()
{
	AActor::DestroySelf();
}

void AGideonMeteor::Initialize(AGideonCharacter* Spawner, const FAttackData& AttackData)
{
	this->Spawner = Spawner;
	ExplosionAttackData = AttackData;
}

void AGideonMeteor::MoveTick()
{
	CurrentSpeed += AccelPerSecond * MoveTickTime;
	if (CurrentSpeed > MaxSpeed)
	{
		CurrentSpeed = MaxSpeed;
	}

	SetActorLocation(GetActorLocation() + GetActorForwardVector() * CurrentSpeed * MoveTickTime);

	static bool bFirstFrame = true;
	static std::vector<ECollisionChannel> CollisionChannel;
	if (bFirstFrame)
	{
		size_t Count = static_cast<size_t>(ECollisionChannel::Count);
		CollisionChannel.reserve(Count);
		for (int i = 0; i < Count; ++i)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			if (Channel == ECollisionChannel::Player || Channel == ECollisionChannel::Enemy || Channel == ECollisionChannel::Pawn)
			{
				continue;
			}
			CollisionChannel.emplace_back(Channel);
		}
	}

	std::vector<AActor*> OverlapActors;
	GPhysicsEngine->SphereOverlapComponents(GetActorLocation(), 30.0f, CollisionChannel, {}, OverlapActors);
	if (!OverlapActors.empty())
	{
		MeteorBomb();
	}
}

void AGideonMeteor::MeteorBomb()
{
	MeteorVFX->Deactivate();
	GEngine->GetTimerManager()->ClearTimer(MoveTimerHandle);

	// 대미지 입히기
	ExplosionAttackData.bIsAttackCenterFixed = true;
	ExplosionAttackData.AttackCenterPos = GetActorLocation();
	Spawner->ApplyDamageToEnemy_Range(ExplosionAttackData, "G_Ult");

	// 후발 이펙트 소환
	const FTransform& CurTransform = RootComponent->GetComponentTransform();
	//GetWorld()->SpawnActor("AGideonMeteorBomb", CurTransform);
}
