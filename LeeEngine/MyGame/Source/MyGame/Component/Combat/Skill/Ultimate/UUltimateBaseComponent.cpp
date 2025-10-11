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

bool UUltimateBaseComponent::TrySkill()
{
	if (!USkillBaseComponent::TrySkill())
	{
		return false;
	}

	// 스킬 사용으로 인한 게이지 감소
	CurrentUltimateGauge = 0.0f;

	if (MyGameCharacter)
	{
		MyGameCharacter->ChangeToUltimateCamera();

		if (const std::shared_ptr<UMyGameWidgetBase>& Widget =  MyGameCharacter->GetCharacterWidget())
		{
			Widget->SetUltimateGauge(CurrentUltimateGauge);
		}
	}

	

	return true;
}

void UUltimateBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::Q, ETriggerEvent::Started, this, &UUltimateBaseComponent::TrySkill);
}

void UUltimateBaseComponent::CoolDown()
{
	CurrentCoolDownTime -= CoolDownTimerRepeatTime;

	if (CurrentCoolDownTime <= 0.0f)
	{
		CurrentCoolDownTime = 0.0f;
		GEngine->GetTimerManager()->ClearTimer(CoolDownTimerHandle);
	}

	if (MyGameCharacter)
	{
		if (const std::shared_ptr<UMyGameWidgetBase>& CharacterWidget = MyGameCharacter->GetCharacterWidget())
		{
			CharacterWidget->SetUltimateCoolDownTime(CurrentCoolDownTime, SkillCoolDownTime);
		}
	}
}

bool UUltimateBaseComponent::CanUseSkill()
{
	return CurrentCoolDownTime <= 0.0f && CurrentUltimateGauge >= MaxUltimateGauge;
}

void UUltimateBaseComponent::ApplyUltimateAttack()
{
}

void UUltimateBaseComponent::AddUltimateGauge(const float NewAddGauge)
{
	assert(MaxUltimateGauge != 0, "MaxUltimateGauge == 0");

	CurrentUltimateGauge = std::min(CurrentUltimateGauge + NewAddGauge, MaxUltimateGauge);

	if (MyGameCharacter)
	{
		if (const std::shared_ptr<UMyGameWidgetBase>& Widget = std::static_pointer_cast<UMyGameWidgetBase>(MyGameCharacter->GetCharacterWidget()))
		{
			Widget->SetUltimateGauge(CurrentUltimateGauge / MaxUltimateGauge);
		}
	}
}
