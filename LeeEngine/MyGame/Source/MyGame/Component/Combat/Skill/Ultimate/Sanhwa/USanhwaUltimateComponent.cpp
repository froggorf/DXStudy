#include "CoreMinimal.h"
#include "USanhwaUltimateComponent.h"

void USanhwaUltimateComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UUltimateBaseComponent::Initialize(MyCharacter);

	SetMaxUltimateGauge(100.0f);
	SetSkillMontagesAndCoolDown(
		{"AM_Sanhwa_Ult"},
		10.0f
	);
}
