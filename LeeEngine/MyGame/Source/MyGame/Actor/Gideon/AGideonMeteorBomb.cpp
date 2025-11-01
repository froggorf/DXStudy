#include "CoreMinimal.h"
#include "AGideonMeteorBomb.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"

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

	GEngine->GetTimerManager()->SetTimer(SelfKillTimerHandle, {this, &AGideonMeteorBomb::DestroySelf}, SelfKillTime);
}

void AGideonMeteorBomb::DestroySelf()
{
	AActor::DestroySelf();
}
