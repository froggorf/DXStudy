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
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void Initialize(AGideonCharacter* Spawner, const FAttackData& AttackData);
	
protected:
	FAttackData ExplosionAttackData;

private: 
	AGideonCharacter* Spawner;

	std::shared_ptr<UNiagaraComponent> MeteorVFX;

	FTimerHandle ThrowTimerHandle;
	// 1000 unit per sec
	static constexpr float SpeedPerSecond = 2000.0f;
	static constexpr float ThrowTimerTickTime = 0.016f;
	void Explosion();
};
