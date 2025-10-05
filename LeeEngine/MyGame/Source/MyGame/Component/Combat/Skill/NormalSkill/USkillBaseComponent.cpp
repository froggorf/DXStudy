#include "CoreMinimal.h"
#include "USkillBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"

USkillBaseComponent::USkillBaseComponent()
{
}

void USkillBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	OwnerCharacter = MyCharacter;
}

bool USkillBaseComponent::Skill()
{
	if (CurrentCoolDownTime > 0.0f)
	{
		// TODO: 인게임 전용 UI 도입도 생각해보기
		MY_LOG(GetName(),EDebugLogLevel::DLL_Warning, GetFunctionName + " Cool Time");
		return false;
	}

	CurrentCoolDownTime = SkillCoolDownTime;
	MY_LOG(GetName(),EDebugLogLevel::DLL_Warning, GetFunctionName);

	GEngine->GetTimerManager()->SetTimer(CoolDownTimerHandle, {this, &USkillBaseComponent::CoolDown}, 
									CoolDownTimerRepeatTime, true, CoolDownTimerRepeatTime);

	return true;
}

void USkillBaseComponent::CoolDown()
{
	CurrentCoolDownTime -= CoolDownTimerRepeatTime;
	MY_LOG(GetName(), EDebugLogLevel::DLL_Display, GetFunctionName + " -> " + std::to_string(CurrentCoolDownTime));

	if (CurrentCoolDownTime <= 0.0f)  // NOLINT(readability-use-std-min-max)
	{
		CurrentCoolDownTime = 0.0f;
		GEngine->GetTimerManager()->ClearTimer(CoolDownTimerHandle);
		MY_LOG(GetName(), EDebugLogLevel::DLL_Display, GetFunctionName + " Finish");
	}
}


void USkillBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::E, ETriggerEvent::Started, this, &USkillBaseComponent::Skill);
}
