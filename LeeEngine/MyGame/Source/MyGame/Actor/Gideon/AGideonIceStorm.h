#pragma once
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class UNiagaraComponent;

class AGideonIceStorm final : public AActor
{
	MY_GENERATE_BODY(AGideonIceStorm)

public:
	AGideonIceStorm();
	~AGideonIceStorm() override = default;
	void Register() override;
	void Tick(float DeltaSeconds) override;
	void Initialize(AGideonCharacter* DamageCauser, const FAttackData& Damage);
	void FindEnemy();

	void DestroySelf() override;

protected:
protected:
	std::shared_ptr<UStaticMeshComponent> StormCollisionComponent;
	std::shared_ptr<UNiagaraComponent> IceStormVFX;
private:
	AGideonCharacter* GideonCharacter;

	FAttackData AttackData;

	FTimerHandle KillSelfTimerHandle;
	static constexpr float KillSelfTime = 6.0f;

};
