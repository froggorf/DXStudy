#include "CoreMinimal.h"
#include "USanhwaUltimateComponent.h"

#include "Engine/FAudioDevice.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
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

void USanhwaUltimateComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UUltimateBaseComponent::Initialize(MyCharacter);

	SetMaxUltimateGauge(100.0f);

	SetSkillAttackData({"AM_Sanhwa_Ult"}, 
		{FAttackData{XMFLOAT3{ 250,250,300},3.0f, -0.0f,30.0f}}
	, 25.0f);

}

void USanhwaUltimateComponent::ApplyUltimateAttack()
{
	UUltimateBaseComponent::ApplyUltimateAttack();

	PlaySound2DByName("SB_SFX_Magic_Ice");
	static_cast<ASanhwaCharacter*>(MyGameCharacter)->CreateIceSpikes(true);
}
