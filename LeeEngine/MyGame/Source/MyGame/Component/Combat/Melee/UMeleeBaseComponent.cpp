#include "CoreMinimal.h"
#include "UMeleeBaseComponent.h"

UMeleeBaseComponent::UMeleeBaseComponent()
{
	
}

void UMeleeBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UCombatBaseComponent::Initialize(MyCharacter);
}

void UMeleeBaseComponent::BasicAttack()
{
	UCombatBaseComponent::BasicAttack();
}

bool UMeleeBaseComponent::HeavyAttack()
{
	return UCombatBaseComponent::HeavyAttack();
}
