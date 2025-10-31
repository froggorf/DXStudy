#pragma once
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class UNiagaraComponent;

class AGideonMeteor : public AActor
{
	MY_GENERATE_BODY(AGideonMeteor)

public:
	AGideonMeteor();
	~AGideonMeteor() override = default;
	void Register() override;
	void BeginPlay() override;
	void OnDestroy() override;
	void DestroySelf() override;
	void Initialize(AGideonCharacter* Spawner, const FAttackData& AttackData);

	void MoveTick();
protected:
	FAttackData ExplosionAttackData;

private: 
	AGideonCharacter* Spawner;

	std::shared_ptr<UNiagaraComponent> MeteorVFX;
	std::shared_ptr<UNiagaraComponent> MeteorBallVFX;
	std::shared_ptr<UNiagaraComponent> CollisionVFX;

	FTimerHandle SelfKillTimerHandle;
	FTimerHandle MoveTimerHandle;

	static constexpr float SelfKillTime = 10.0f;
	static constexpr float MoveTickTime = 0.030f; //약 30번 (1초)
};
