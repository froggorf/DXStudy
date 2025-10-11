#include "CoreMinimal.h"
#include "USanhwaUltimateComponent.h"

#include "Engine/RenderCore/EditorScene.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Character/ASanhwaCharacter.h"

void USanhwaUltimateComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UUltimateBaseComponent::Initialize(MyCharacter);

	SetMaxUltimateGauge(100.0f);
	SetSkillMontagesAndCoolDown(
		{"AM_Sanhwa_Ult"},
		25.0f
	);
}

void USanhwaUltimateComponent::ApplyUltimateAttack()
{
	UUltimateBaseComponent::ApplyUltimateAttack();

	static_cast<ASanhwaCharacter*>(MyGameCharacter)->CreateIceSpikes(true);
}
