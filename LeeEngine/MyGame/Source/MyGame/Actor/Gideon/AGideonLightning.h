#pragma once
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class UNiagaraComponent;

class AGideonLightning : public AActor
{
	MY_GENERATE_BODY(AGideonLightning)

public:
	AGideonLightning();
	~AGideonLightning() override = default;
	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void Initialize(AGideonCharacter* DamageCauser, const FAttackData& Damage, UINT ApplyCount, UINT ChargeLevel);
	void FindEnemy();

	void DestroySelf() override;

	void StartAttack();
protected:
	XMFLOAT3 GetRecentChainPos();
protected:
	// 적군이 중간에 사라질 경우를 대비하여 (weak_ptr로 데이터를 받지 않음)
	// 적군을 파악하는 타이밍에 위치를 저장하여 위치를 통해 업데이트를 진행
	std::vector<XMFLOAT3> LightningChainPos;
	std::vector<AActor*> TargetEnemies;

	std::shared_ptr<UNiagaraComponent> LightningChainVFX;
	bool bVFXMoveStart = false;
	float VFXMoveValue = 0.0f;
private:
	AGideonCharacter* GideonCharacter;
	UINT ApplyCount = 1;

	FAttackData AttackData;
	UINT ChargeLevel = 0;

	static constexpr float ChainMaxDistance = 1000.0f;

	FTimerHandle KillSelfTimerHandle;
	static constexpr float KillSelfTime = 6.0f;

	static constexpr float ChainToNextEnemyTime = 0.1f;
};
