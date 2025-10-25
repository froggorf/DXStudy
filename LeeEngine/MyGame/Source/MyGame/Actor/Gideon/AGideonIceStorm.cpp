#include "CoreMinimal.h"
#include "AGideonIceStorm.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/World/UWorld.h"

AGideonIceStorm::AGideonIceStorm()
{
	IceStormVFX = std::make_shared<UNiagaraComponent>();
	IceStormVFX->SetupAttachment(GetRootComponent());
	IceStormVFX->SetRelativeLocation({0.0f,0.0f,0.0f});

	StormCollisionComponent = std::make_shared<UStaticMeshComponent>();
	StormCollisionComponent->SetupAttachment(GetRootComponent());
	StormCollisionComponent->SetRelativeScale3D({20,20,20});
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

	IceStormVFX->SetNiagaraAsset(UNiagaraSystem::GetNiagaraAsset("NS_IceStorm"));
	IceStormVFX->Activate();
	AActor::Register();

}

void AGideonIceStorm::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

}

void AGideonIceStorm::Initialize(AGideonCharacter* DamageCauser, const FAttackData& Damage)
{
	GideonCharacter = DamageCauser;
	
	AttackData = Damage;

	GEngine->GetTimerManager()->SetTimer(KillSelfTimerHandle, {this, &AGideonIceStorm::DestroySelf}, KillSelfTime);

}

void AGideonIceStorm::FindEnemy()
{
	std::vector<AActor*> OverlapEnemies;

	if (!OverlapEnemies.empty())
	{
		// TODO: 대미지 입히기
	}

}

void AGideonIceStorm::DestroySelf()
{
	AActor::DestroySelf();
}

