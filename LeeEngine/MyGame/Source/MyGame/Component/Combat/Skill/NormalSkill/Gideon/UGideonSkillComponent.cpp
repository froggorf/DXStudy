#include "CoreMinimal.h"
#include "UGideonSkillComponent.h"

#include "MyGame/Character/Player/ASanhwaCharacter.h"

void UGideonSkillComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	USkillBaseComponent::Initialize(MyCharacter);

	SetSkillAttackData({"AM_Gideon_Skill"}, 
	{FAttackData{XMFLOAT3{}, 3.0f, 0.0f, 30.0f}},
	15.0f);
}

void UGideonSkillComponent::ApplySkillAttack()
{
	USkillBaseComponent::ApplySkillAttack();


}

