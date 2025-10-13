#include "CoreMinimal.h"
#include "UHealthComponent.h"

#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"
#include "MyGame/Widget/Health/UHealthWidgetBase.h"

void UHealthComponent::SetMaxHealth(float NewMaxHealth, bool bSetHealthMax)
{
	MaxHealth = NewMaxHealth;

	if (bSetHealthMax)
	{
		CurrentHealth = MaxHealth;
	}
	CurrentHealth = bSetHealthMax? MaxHealth : std::min(NewMaxHealth, CurrentHealth);
	MarkHealthToWidget();
}

float UHealthComponent::ApplyDamage(float AppliedDamage)
{
	CurrentHealth = max(CurrentHealth - AppliedDamage, 0.0f);

	MarkHealthToWidget();

	return CurrentHealth;
}

void UHealthComponent::MarkHealthToWidget()
{
	if (const std::shared_ptr<UHealthWidgetBase>& Widget = HealthWidget.lock())
	{
		Widget->SetHealthBarPercent(CurrentHealth, MaxHealth);
	}
}
