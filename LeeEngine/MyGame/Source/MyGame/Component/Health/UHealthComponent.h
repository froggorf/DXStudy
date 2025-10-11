#pragma once
#include "Engine/Components/UActorComponent.h"

class UHealthComponent : public UActorComponent
{
	MY_GENERATE_BODY(UHealthComponent)

public:
	void SetHealth(float NewMaxHealth);
	float GetHealthPercent() const { if (MaxHealth == 0.0f) {return 0.0f;} return CurrentHealth / MaxHealth;}

	// 공격을 받은 후의 체력을 반환
	float ApplyDamage(float AppliedDamage);
protected:


private:
	float MaxHealth = 300.0f;
	float CurrentHealth = 0.0f;
};