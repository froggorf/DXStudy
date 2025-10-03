#include "CoreMinimal.h"
#include "USanhwaCombatComponent.h"

#include "Engine/RenderCore/EditorScene.h"

USanhwaCombatComponent::USanhwaCombatComponent()
{
	BasicAttackMoveDistance = {
		200, 200, 200, -200, 500
	};
}

void USanhwaCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UMeleeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackMontages({
		"AM_Sanhwa_BasicAttack0",
		"AM_Sanhwa_BasicAttack1",
		"AM_Sanhwa_BasicAttack2",
		"AM_Sanhwa_BasicAttack3",
		"AM_Sanhwa_BasicAttack4",
	});
}

void USanhwaCombatComponent::BasicAttack()
{
	UMeleeBaseComponent::BasicAttack();
}

