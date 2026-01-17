#pragma once
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class UNiagaraComponent;

class AGideonFireBall : public AActor
{
	MY_GENERATE_BODY(AGideonFireBall)

public:
	AGideonFireBall();
	~AGideonFireBall() override = default;
	void Register() override;
	void BeginPlay() override;

	void Initialize(AGideonCharacter* Spawner, const XMFLOAT3& TargetPosition, const FAttackData& AttackData);


	void FlyFireBall();

	void OnHit(UShapeComponent* ShapeComp, AActor* HitActor, UShapeComponent* HitComponent, const FHitResult& Result);
protected:
	FAttackData ExplosionAttackData;

private: 
	std::shared_ptr<UStaticMeshComponent> SM_FireBall;
	AGideonCharacter* Spawner;
	XMFLOAT3 StartPosition;
	XMFLOAT3 TargetPosition;

	std::shared_ptr<UNiagaraComponent> FireVFX;

	FTimerHandle ThrowTimerHandle;
	// 1000 unit per sec
	static constexpr float SpeedPerSecond = 2000.0f;
	static constexpr float ThrowTimerTickTime = 0.016f;
	float FireSoundTimer = 0.0f;
	static constexpr float FireSoundInterval = 0.1f;
	void Explosion();
};
