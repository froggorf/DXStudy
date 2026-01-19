#include "CoreMinimal.h"
#include "UMeleeBaseComponent.h"

UMeleeBaseComponent::UMeleeBaseComponent()
{
	
}

void UMeleeBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UCombatBaseComponent::Initialize(MyCharacter);
}

bool UMeleeBaseComponent::HeavyAttack()
{
	return UCombatBaseComponent::HeavyAttack();
}
