#include "CoreMinimal.h"
#include "USanhwaSkillComponent.h"

#include "MyGame/Character/ASanhwaCharacter.h"

USanhwaSkillComponent::USanhwaSkillComponent()
{
}

void USanhwaSkillComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	USkillBaseComponent::Initialize(MyCharacter);

	// 동일한 애니메이션
	SetSkillMontagesAndCoolDown({"AM_Sanhwa_Skill"}, 7.0f);
	SetSkillMoveDistance({-300,-300});
}

void USanhwaSkillComponent::ApplySkillAttack()
{
	USkillBaseComponent::ApplySkillAttack();

	static_cast<ASanhwaCharacter*>(MyGameCharacter)->CreateIceSpikes(false);
	// TODO: 대미지 적용하기
}
