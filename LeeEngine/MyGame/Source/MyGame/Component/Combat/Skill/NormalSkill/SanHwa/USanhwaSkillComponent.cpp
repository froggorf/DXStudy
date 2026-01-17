#include "CoreMinimal.h"
#include "USanhwaSkillComponent.h"

#include "Engine/FAudioDevice.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"

namespace
{
	void PlaySound2DByName(const char* SoundName)
	{
		if (!GAudioDevice || !SoundName || SoundName[0] == '\0')
		{
			return;
		}

		if (const std::shared_ptr<USoundBase>& Sound = USoundBase::GetSoundAsset(SoundName))
		{
			GAudioDevice->PlaySound2D(Sound);
		}
	}
}

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
		PlaySound2DByName("SB_SFX_Magic_Ice");
		Sanhwa->CreateIceSpikes(false);
	}
}
