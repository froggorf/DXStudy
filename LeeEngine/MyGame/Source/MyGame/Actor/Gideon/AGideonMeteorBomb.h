#pragma once
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class UDecalComponent;
class UNiagaraComponent;

class AGideonMeteorBomb : public AActor
{
	MY_GENERATE_BODY(AGideonMeteorBomb)

public:
	AGideonMeteorBomb();
	~AGideonMeteorBomb() override = default;
	void Register() override;
	void BeginPlay() override;
	void DestroySelf() override;

private: 
	std::shared_ptr<UNiagaraComponent> CollisionVFX;
	std::shared_ptr<UDecalComponent> GroundDecalComp;

	FTimerHandle SelfKillTimerHandle;
	static constexpr float SelfKillTime = 5.0f;
};
