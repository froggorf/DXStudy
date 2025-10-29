#include "CoreMinimal.h"
#include "AGideonMeteor.h"

#include "Engine/Components/UNiagaraComponent.h"

AGideonMeteor::AGideonMeteor()
{
	MeteorVFX = std::make_shared<UNiagaraComponent>();
	MeteorVFX->SetupAttachment(GetRootComponent());
	const std::shared_ptr<UNiagaraSystem>& System = UNiagaraSystem::GetNiagaraAsset("NS_Meteor");
	MeteorVFX->SetNiagaraAsset(System);
}

void AGideonMeteor::BeginPlay()
{
	AActor::BeginPlay();

}

void AGideonMeteor::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * 1000*DeltaSeconds);
}

void AGideonMeteor::Initialize(AGideonCharacter* Spawner, const FAttackData& AttackData)
{
	this->Spawner = Spawner;
	ExplosionAttackData = AttackData;
}
