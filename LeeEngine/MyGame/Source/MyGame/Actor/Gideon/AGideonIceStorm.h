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

	void SetLifeTime_Timer();

protected:
protected:
	std::shared_ptr<UStaticMeshComponent> StormCollisionComponent;
	std::shared_ptr<UNiagaraComponent> IceStormVFX;
private:
	AGideonCharacter* GideonCharacter;

	FAttackData AttackData;

	FTimerHandle KillSelfTimerHandle;
	float CurrentLifeTime = 0.0f;
	static constexpr float SetScaleTimerTickTime = 0.05f;
	static constexpr float KillSelfTime = 8.0f;

	FTimerHandle FindEnemyAndApplyDamageTimerHandle;
	static constexpr float FindEnemyAndApplyDamageTickTime = 0.8f;
	static constexpr float MoveSpeedPerSec = 500.0f;
	static constexpr float ApplyDamageDistance = 300.0f;

};
