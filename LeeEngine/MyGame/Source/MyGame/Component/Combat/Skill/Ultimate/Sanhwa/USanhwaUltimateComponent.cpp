#include "CoreMinimal.h"
#include "USanhwaUltimateComponent.h"

#include "MyGame/Actor/Sanhwa/ASanhwaIceBase.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"

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

void USanhwaUltimateComponent::SetIceSpikeCollision(ECollisionEnabled NewCollision)
{
	std::vector<std::shared_ptr<ASanhwaIceSpikeBase>> IceSpikes;
	static_cast<ASanhwaCharacter*>(MyGameCharacter)->GetIceSpikes(IceSpikes);
	for (std::shared_ptr<ASanhwaIceSpikeBase>& IceSpike : IceSpikes)
	{
		IceSpike->SetCollision(NewCollision);
	}
}
