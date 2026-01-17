#include "CoreMinimal.h"
#include "AGideonMeteorBomb.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/FAudioDevice.h"


AGideonMeteorBomb::AGideonMeteorBomb()
{
	CollisionVFX = std::make_shared<UNiagaraComponent>();
	CollisionVFX->SetupAttachment(GetRootComponent());

	//GroundDecalComp = std::make_shared<UDecalComponent>();
	//const std::shared_ptr<UMaterialInterface>& GroundDecalMaterial = UMaterial::GetMaterialCache("");
	//GroundDecalComp->SetupAttachment(GetRootComponent());
	//GroundDecalComp->SetDecalMaterial();
	//GroundDecalComp->SetIsLight(false);
	//GroundDecalComp->SetActive(true);
}

void AGideonMeteorBomb::Register()
{
	static const std::shared_ptr<UNiagaraSystem>& NS_Collision = UNiagaraSystem::GetNiagaraAsset("NS_MeteorCollision");
	CollisionVFX->SetNiagaraAsset(NS_Collision);

	AActor::Register();
}

void AGideonMeteorBomb::BeginPlay()
{
	AActor::BeginPlay();

	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Explosion_01");
	GEngine->GetTimerManager()->SetTimer(SelfKillTimerHandle, {this, &AGideonMeteorBomb::DestroySelf}, SelfKillTime);
}

void AGideonMeteorBomb::DestroySelf()
{
	AActor::DestroySelf();
}
