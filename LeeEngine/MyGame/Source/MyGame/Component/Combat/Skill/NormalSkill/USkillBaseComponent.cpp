#include "CoreMinimal.h"
#include "USkillBaseComponent.h"

#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

USkillBaseComponent::USkillBaseComponent()
{
}

void USkillBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	MyGameCharacter = MyCharacter;
}

bool USkillBaseComponent::TrySkill()
{
	if (!MyGameCharacter)
	{
		return false;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = MyGameCharacter->GetAnimInstance();
	if (!AnimInstance || AnimInstance->IsPlayingMontage())
	{
		MY_LOG(GetName(),EDebugLogLevel::DLL_Warning, GetFunctionName + " !AnimInstance || Playing Montage");
		return false;
	}

	LastPressKeyTime = GEngine->GetTimeSeconds();

	if (!CanUseSkill())
	{
		// TODO: 인게임 전용 UI 도입도 생각해보기
		MY_LOG(GetName(),EDebugLogLevel::DLL_Warning, GetFunctionName + " Cool Time");
		return false;
	}

	DoSkill(0);
	return true;
}

void USkillBaseComponent::CheckDoNextSkill()
{
	CurrentCheckDoNextSkillTime += 0.01f;
	if (CurrentCheckDoNextSkillTime > CheckDoNextSkillTime)
	{
		GEngine->GetTimerManager()->ClearTimer(CheckDoNextSkillTimerHandle);
		bIsCurrentPlayingSkill = false;
		return;
	}

	float CurrentTime = GEngine->GetTimeSeconds();
	// 키 입력이 진행됐었다면,
	if (LastPressKeyTime + 0.05f > CurrentTime && CurrentSkillCombo < SkillAnimMontages.size()-1)
	{
		DoSkill(CurrentSkillCombo + 1);
	}
}

void USkillBaseComponent::DoSkill(UINT SkillIndex)
{
	const std::shared_ptr<UAnimInstance>& AnimInstance = MyGameCharacter->GetAnimInstance();

	// 스킬 콤보 0번째에만 쿨타임 설정
	if (SkillIndex == 0)
	{
		CurrentCoolDownTime = SkillCoolDownTime;

		GEngine->GetTimerManager()->SetTimer(CoolDownTimerHandle, {this, &USkillBaseComponent::CoolDown}, 
			CoolDownTimerRepeatTime, true, CoolDownTimerRepeatTime);
	}

	bIsCurrentPlayingSkill = true;

	// SkillIndex 번째 스킬 재생 및 델리게이트 진행
	AnimInstance->Montage_Play(SkillAnimMontages[SkillIndex], 0, {}, {}, {this, &USkillBaseComponent::OnSkillBlendOut});
	if (SkillIndex < SkillStartDelegates.size())
	{
		SkillStartDelegates[SkillIndex].Broadcast();
	}

	CurrentSkillCombo = SkillIndex;
}

void USkillBaseComponent::OnSkillBlendOut()
{
	// 다음 콤보가 있으면 스킬 사용할지 측정하는 타이머 설정하고
	if (CurrentSkillCombo < SkillAnimMontages.size() - 1)
	{
		CurrentCheckDoNextSkillTime = 0.0f;
		GEngine->GetTimerManager()->SetTimer(CheckDoNextSkillTimerHandle, {this, &USkillBaseComponent::CheckDoNextSkill}, 0.0f, true, 0.01f);
	}
	else
	{
		CurrentSkillCombo = 0;
		bIsCurrentPlayingSkill = false;
	}

	// BlendOut Delegate 실행
	if (CurrentSkillCombo < SkillBlendOutDelegates.size())
	{
		SkillBlendOutDelegates[CurrentSkillCombo].Broadcast();
	}
}

bool USkillBaseComponent::CanUseSkill()
{
	for (size_t i = 0; i < SkillAnimMontages.size(); ++i)
	{
		if (!SkillAnimMontages[i])
		{
			MY_LOG("No Resource" , EDebugLogLevel::DLL_Warning, GetFunctionName + "-> No Montage Yet");
			return false;
		}
	}

	return CurrentCoolDownTime <= 0.0f && !bIsCurrentPlayingSkill;
}

void USkillBaseComponent::CoolDown()
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
			CharacterWidget->SetSkillCoolDownTime(CurrentCoolDownTime, SkillCoolDownTime);
		}
	}
}

float USkillBaseComponent::GetSkillAnimLength(size_t SkillIndex) const
{
	if (SkillIndex >= SkillAnimMontages.size())
	{
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
		std::string ErrorText = GetFunctionName + " - Wrong Index";
		assert(nullptr && ErrorText.data());
#else
		return 0.0f;
#endif
	}
	return SkillAnimMontages[SkillIndex]->GetPlayLength();
}

void USkillBaseComponent::SetSkillMontagesAndCoolDown(const std::vector<std::string>& NewMontageNames, const float NewSkillCooldown)
{
	size_t Size = NewMontageNames.size();
	SkillAnimMontages.clear();
	SkillAnimMontages.resize(Size);
	for (size_t Index = 0; Index < Size; ++Index)
	{
		AssetManager::GetAsyncAssetCache(NewMontageNames[Index],[this, Index](std::shared_ptr<UObject> Object)
			{
				SkillAnimMontages[Index] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (!SkillAnimMontages[Index])
				{
					MY_LOG("SetSkillMontage", EDebugLogLevel::DLL_Error, "No valid name");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
					// 개발 중 테스트를 위하여 assert
					assert(nullptr&&"Not exist Skill Montage");
#endif
				}
			});
	}

	SkillCoolDownTime = NewSkillCooldown;
}

void USkillBaseComponent::SetSkillMoveDistance(const std::vector<float>& NewMoveDistance)
{
	size_t NewSize = NewMoveDistance.size();
	SkillMoveDistance.resize(NewSize);
	for (size_t i = 0; i < NewSize; ++i)
	{
		SkillMoveDistance[i] = NewMoveDistance[i];
	}
}


void USkillBaseComponent::SetSkillDelegates(const std::vector<Delegate<>>& NewStartDelegates, const std::vector<Delegate<>>& NewBlendOutDelegates)
{
	// StartDelegate	
	size_t StartSize = NewStartDelegates.size();
	SkillStartDelegates.resize(StartSize);
	for (size_t i = 0; i < StartSize; ++i)
	{
		SkillStartDelegates[i] = std::move(NewStartDelegates[i]);	
	}

	size_t BlendOutSize = NewBlendOutDelegates.size();
	SkillBlendOutDelegates.resize(BlendOutSize);
	for (size_t i = 0; i < BlendOutSize; ++i)
	{
		SkillBlendOutDelegates[i] = std::move(NewBlendOutDelegates[i]);
	}
}

void USkillBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::E, ETriggerEvent::Started, this, &USkillBaseComponent::TrySkill);
}
