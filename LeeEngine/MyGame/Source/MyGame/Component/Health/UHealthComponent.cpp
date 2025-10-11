#include "CoreMinimal.h"
#include "UHealthComponent.h"

#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

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
	if (!MyGameCharacter)
	{
		MyGameCharacter = dynamic_cast<AMyGameCharacterBase*>(GetOwner());
	}

	if (const std::shared_ptr<UMyGameWidgetBase>& Widget = MyGameCharacter->GetCharacterWidget())
	{
		Widget->SetHealthBarWidget(CurrentHealth, MaxHealth);
	}
}
