#include "CoreMinimal.h"
#include "UUltimateBaseComponent.h"
#include "Engine/Class/Framework/UPlayerInput.h"
#include "MyGame/Character/AMyGameCharacterBase.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

UUltimateBaseComponent::UUltimateBaseComponent()
{
}

void UUltimateBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	USkillBaseComponent::Initialize(MyCharacter);
}

void UUltimateBaseComponent::TrySkill()
{
	USkillBaseComponent::TrySkill();

	// 스킬 사용으로 인한 게이지 감소
	CurrentUltimateGauge = 0.0f;
}

void UUltimateBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::Q, ETriggerEvent::Started, this, &UUltimateBaseComponent::TrySkill);
}

bool UUltimateBaseComponent::CanUseSkill()
{
	return USkillBaseComponent::CanUseSkill() && CurrentUltimateGauge >= MaxUltimateGauge;
}

void UUltimateBaseComponent::AddUltimateGauge(const float NewAddGauge)
{
	assert(MaxUltimateGauge != 0, "MaxUltimateGauge == 0");

	CurrentUltimateGauge = std::min(CurrentUltimateGauge + NewAddGauge, MaxUltimateGauge);

	if (OwnerCharacter)
	{
		if (const std::shared_ptr<UMyGameWidgetBase>& Widget = std::static_pointer_cast<UMyGameWidgetBase>(OwnerCharacter->GetCharacterWidget()))
		{
			Widget->SetUltimateGauge(CurrentUltimateGauge / MaxUltimateGauge);
		}
	}
}
