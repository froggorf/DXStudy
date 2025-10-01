#include "CoreMinimal.h"
#include "UCombatBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/AMyGameCharacterBase.h"

UCombatBaseComponent::UCombatBaseComponent()
{
	
}

void UCombatBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	OwnerCharacter = MyCharacter;
}

void UCombatBaseComponent::BasicAttack()
{
	if (bIsBasicAttacking)
	{
		MY_LOG("-----------" , EDebugLogLevel::DLL_Warning, "time change");
		LastBasicAttackClickedTime = GEngine->GetTimeSeconds();
		return;
	}

	// 진행중이 아니면 1타 공격부터 시작하고
	// MontageEnd를 통해서 공격을 계속 진행
	if (const std::shared_ptr<UAnimInstance>& AnimInstance =  OwnerCharacter->GetAnimInstance())
	{
		AnimInstance->Montage_Play(BasicAttackMontages[0], 0, {this, &UCombatBaseComponent::BasicAttackEnded});
	}
	bIsBasicAttacking = true;
	MY_LOG("-----------" , EDebugLogLevel::DLL_Warning, "Do First");
}

void UCombatBaseComponent::BasicAttackEnded()
{
	float CurrentTime = GEngine->GetTimeSeconds();
	if (LastBasicAttackClickedTime + BasicAttackApplyTime >= CurrentTime)
	{
		CurrentBasicAttackCombo = (CurrentBasicAttackCombo + 1) % static_cast<UINT>(BasicAttackMontages.size());
		if (const std::shared_ptr<UAnimInstance>& AnimInstance =  OwnerCharacter->GetAnimInstance())
		{
			AnimInstance->Montage_Play(BasicAttackMontages[CurrentBasicAttackCombo], 0, {this, &UCombatBaseComponent::BasicAttackEnded});
		}
		MY_LOG("-----------" , EDebugLogLevel::DLL_Warning, std::to_string(CurrentBasicAttackCombo));
	}
	else
	{
		CurrentBasicAttackCombo = 0;
		bIsBasicAttacking = false;
		MY_LOG("-----------" , EDebugLogLevel::DLL_Warning, "Clear");
	}
}

void UCombatBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Started, this, &UCombatBaseComponent::BasicAttack);
}

void UCombatBaseComponent::SetBasicAttackMontages(const std::vector<std::string>& NewMontagesName)
{
	size_t Size = NewMontagesName.size();
	BasicAttackMontages.clear();
	BasicAttackMontages.resize(NewMontagesName.size());
	for (size_t Index = 0; Index < Size; ++Index)
	{
		AssetManager::GetAsyncAssetCache(NewMontagesName[Index],[this, Index](std::shared_ptr<UObject> Object)
		{
			BasicAttackMontages[Index] = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!BasicAttackMontages[Index])
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", BasicAttackMontages (Index) Montage Not exist - "+ std::to_string(Index));
#ifdef WITH_EDITOR
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist BasicAttackMontages");
#endif
			}
		});
	}
	
}

