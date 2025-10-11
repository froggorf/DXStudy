#include "CoreMinimal.h"
#include "UHealthComponent.h"

void UHealthComponent::SetHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	CurrentHealth = std::min(NewMaxHealth, CurrentHealth);
}

float UHealthComponent::ApplyDamage(float AppliedDamage)
{
	CurrentHealth = max(CurrentHealth - AppliedDamage, 0.0f);

	return CurrentHealth;
}
