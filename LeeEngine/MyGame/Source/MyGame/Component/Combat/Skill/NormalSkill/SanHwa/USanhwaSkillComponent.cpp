#include "CoreMinimal.h"
#include "USanhwaSkillComponent.h"

#include "MyGame/Character/Player/ASanhwaCharacter.h"

USanhwaSkillComponent::USanhwaSkillComponent()
{
}

void USanhwaSkillComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	USkillBaseComponent::Initialize(MyCharacter);

	// 동일한 애니메이션
	SetSkillAttackData({"AM_Sanhwa_Skill"}, 
	{FAttackData{XMFLOAT3{ 250,250,300},2.0f, -50.0f,20.0f}}
	, 7.0f);
}

void USanhwaSkillComponent::ApplySkillAttack()
{
	USkillBaseComponent::ApplySkillAttack();

	if (ASanhwaCharacter* Sanhwa = dynamic_cast<ASanhwaCharacter*>(MyGameCharacter))
	{
		Sanhwa->CreateIceSpikes(false);
	}
}
