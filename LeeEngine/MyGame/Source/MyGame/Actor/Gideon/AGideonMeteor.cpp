#include "CoreMinimal.h"
#include "AGideonMeteor.h"

#include "Engine/Components/UNiagaraComponent.h"

AGideonMeteor::AGideonMeteor()
{
	MeteorVFX = std::make_shared<UNiagaraComponent>();
	MeteorVFX->SetupAttachment(GetRootComponent());

	MeteorBallVFX = std::make_shared<UNiagaraComponent>();
	MeteorBallVFX->SetupAttachment(GetRootComponent());
	MeteorBallVFX->SetRelativeLocation({0.0f, 0.0f, 15.0f});


	CollisionVFX = std::make_shared<UNiagaraComponent>();
	CollisionVFX->SetupAttachment(GetRootComponent());
}

void AGideonMeteor::Register()
{
	static const std::shared_ptr<UNiagaraSystem>& NS_Meteor = UNiagaraSystem::GetNiagaraAsset("NS_Meteor");
	MeteorVFX->SetNiagaraAsset(NS_Meteor);

	static const std::shared_ptr<UNiagaraSystem>& NS_MeteorBall = UNiagaraSystem::GetNiagaraAsset("NS_MeteorBall");
	MeteorBallVFX->SetNiagaraAsset(NS_MeteorBall);

	static const std::shared_ptr<UNiagaraSystem>& NS_Collision = UNiagaraSystem::GetNiagaraAsset("NS_MeteorCollision");
	CollisionVFX->SetNiagaraAsset(NS_Collision);

	AActor::Register();
}

void AGideonMeteor::BeginPlay()
{
	AActor::BeginPlay();

	CollisionVFX->Deactivate();

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
	static float MoveDistancePerTick = 1000.0f;
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * MoveDistancePerTick * MoveTickTime);

	static bool bFirstFrame = true;
	static std::vector<ECollisionChannel> CollisionChannel;
	if (bFirstFrame)
	{
		size_t Count = static_cast<size_t>(ECollisionChannel::Count);
		CollisionChannel.reserve(Count);
		for (int i = 0; i < Count; ++i)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			if (Channel == ECollisionChannel::Player || Channel == ECollisionChannel::Enemy)
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
		MeteorVFX->Deactivate();
		CollisionVFX->Activate();
		GEngine->GetTimerManager()->ClearTimer(MoveTimerHandle);	
	}
}
