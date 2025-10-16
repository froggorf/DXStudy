#include "CoreMinimal.h"
#include "UGideonCombatComponent.h"

UGideonCombatComponent::UGideonCombatComponent()
{
}

void UGideonCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	URangeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackData({
		"AM_Gideon_BasicAttack0",
		"AM_Gideon_BasicAttack1",
		"AM_Gideon_BasicAttack2",
	},
	{
		FAttackData{{10,10,10}, 1.0f, 0.0f, 10.0f, true},
		FAttackData{{20,20,20}, 1.25f, 0.0f, 10.0f, true},
		FAttackData{{30,30,30}, 1.5f, 0.0f, 15.0f, true},
	});
}

void UGideonCombatComponent::BasicAttack()
{
	URangeBaseComponent::BasicAttack();
}

bool UGideonCombatComponent::HeavyAttack()
{
	return URangeBaseComponent::HeavyAttack();
}

void UGideonCombatComponent::HeavyAttackMouseReleased()
{
	URangeBaseComponent::HeavyAttackMouseReleased();
}
