#pragma once
#include "Engine/Components/UActorComponent.h"

class UHealthWidgetBase;
class AMyGameCharacterBase;

class UHealthComponent : public UActorComponent
{
	MY_GENERATE_BODY(UHealthComponent)

public:
	void SetMaxHealth(float NewMaxHealth, bool bSetHealthMax = false);
	float GetHealthPercent() const { if (MaxHealth == 0.0f) {return 0.0f;} return CurrentHealth / MaxHealth;}
	void RefreshHealthWidget();

	// 공격을 받은 후의 체력을 반환
	float ApplyDamage(float AppliedDamage);
	void SetHealthWidget(const std::shared_ptr<UHealthWidgetBase>& TargetHealthWidget) {HealthWidget = TargetHealthWidget;}
protected:
	virtual void MarkHealthToWidget();
	std::weak_ptr<UHealthWidgetBase> HealthWidget;
	float MaxHealth = 300.0f;
	float CurrentHealth = 0.0f;
private:
};
