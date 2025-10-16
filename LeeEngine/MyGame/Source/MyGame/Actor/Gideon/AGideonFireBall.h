#pragma once
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class AGideonFireBall : public AActor
{
	MY_GENERATE_BODY(AGideonFireBall)

public:
	AGideonFireBall();
	~AGideonFireBall() override = default;
	void Register() override;
	void Tick(float DeltaSeconds) override;

	void Initialize(AGideonCharacter* Spawner, const XMFLOAT3& TargetPosition, const FAttackData& AttackData);

protected:
	FAttackData ExplosionAttackData;

private: 
	std::shared_ptr<UStaticMeshComponent> SM_FireBall;
	AGideonCharacter* Spawner;
	XMFLOAT3 StartPosition;
	XMFLOAT3 TargetPosition;
	float CurrentTime = 0.0f;

	// Gideon 캐릭터가 발사 후 몹에게 까지 날아가는데 걸리는 시간
	static constexpr float ThrowTime = 0.5f;

	void Explosion();
};
