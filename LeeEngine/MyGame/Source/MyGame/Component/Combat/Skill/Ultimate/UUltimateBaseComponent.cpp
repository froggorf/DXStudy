#include "CoreMinimal.h"
#include "UUltimateBaseComponent.h"
#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/FAudioDevice.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
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

	PlaySound2DByName("SB_SFX_Magic_Ultimate");

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
	assert(MaxUltimateGauge != 0 && "MaxUltimateGauge == 0");

	const float PrevGauge = CurrentUltimateGauge;
	CurrentUltimateGauge = std::min(CurrentUltimateGauge + NewAddGauge, MaxUltimateGauge);
	if (PrevGauge < MaxUltimateGauge && CurrentUltimateGauge >= MaxUltimateGauge)
	{
		PlaySound2DByName("SB_SFX_UI_Notify");
	}

	if (MyGameCharacter)
	{
		if (const std::shared_ptr<UMyGameWidgetBase>& Widget = std::static_pointer_cast<UMyGameWidgetBase>(MyGameCharacter->GetCharacterWidget()))
		{
			Widget->SetUltimateGauge(CurrentUltimateGauge / MaxUltimateGauge);
		}
	}
}
