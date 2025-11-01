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

	// 타이머 틱 마다 메테오를 이동시키는 함수
	// (매 타이머틱마다 오버랩을 통해 충돌체크)
	void MoveTick();
	// 지면에 부딪힐 시 발생하는 함수
	void MeteorBomb();
protected:
	FAttackData ExplosionAttackData;

private: 
	AGideonCharacter* Spawner;

	std::shared_ptr<UNiagaraComponent> MeteorVFX;
	// 기존에는 나이아가라를 사용했으나 회전값을 적절히 사용할 방법이 존재하지 않아 다음과같이 변경
	std::shared_ptr<UStaticMeshComponent> MeteorBallComp;

	FTimerHandle SelfKillTimerHandle;
	FTimerHandle MoveTimerHandle;

	static constexpr float SelfKillTime = 10.0f;
	static constexpr float MoveTickTime = 0.030f; //약 30번 (1초)
};
