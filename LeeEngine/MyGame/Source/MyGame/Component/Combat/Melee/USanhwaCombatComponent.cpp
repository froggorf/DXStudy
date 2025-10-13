#include "CoreMinimal.h"
#include "USanhwaCombatComponent.h"

#include "Engine/RenderCore/EditorScene.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Component/Combat/Skill/Ultimate/Sanhwa/USanhwaUltimateComponent.h"
#include "MyGame/Widget/Sanhwa/USanhwaWidget.h"

USanhwaCombatComponent::USanhwaCombatComponent()
{
}

void USanhwaCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	UMeleeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackData({
		"AM_Sanhwa_BasicAttack0",
		"AM_Sanhwa_BasicAttack1",
		"AM_Sanhwa_BasicAttack2",
		"AM_Sanhwa_BasicAttack3",
		"AM_Sanhwa_BasicAttack4",
	},	{
		FAttackData{{250,250,250}, 1.0f, 200.0, 5.0f},
		FAttackData{{250,250,250}, 1.0f,200.0, 5.0f},
		FAttackData{{250,250,250}, 1.1f,200.0,5.0f},
		FAttackData{{250,250,250}, 0.8f,-50.0, 5.0f},
		FAttackData{{250,250,250}, 1.5f,500.0,15.0f}
	});

	/// AM_Sanhwa_Heavy_Press
	AssetManager::GetAsyncAssetCache("AM_Sanhwa_Heavy_Press",[this](std::shared_ptr<UObject> Object)
		{
			AM_HeavyAttack_Press = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!AM_HeavyAttack_Press)
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", Heavy Montage Not exist");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist HeavyPress");
#endif
			}
		});

	/// AM_Sanhwa_Heavy_Release
	AssetManager::GetAsyncAssetCache("AM_Sanhwa_Heavy_Release",[this](std::shared_ptr<UObject> Object)
		{
			AM_HeavyAttack_Release = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!AM_HeavyAttack_Release)
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", Heavy Montage Not exist");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist HeavyRelease");
#endif
			}
		});

	/// AM_Sanhwa_Heavy_Attack
	AssetManager::GetAsyncAssetCache("AM_Sanhwa_Heavy_Attack",[this](std::shared_ptr<UObject> Object)
		{
			AM_HeavyAttack_Attack = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!AM_HeavyAttack_Attack)
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, GetFunctionName + ", Heavy Montage Not exist");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist HeavyPress");
#endif
			}
		});

}

void USanhwaCombatComponent::BasicAttack()
{
	UMeleeBaseComponent::BasicAttack();
}

void USanhwaCombatComponent::HeavyAttack()
{
	if (!MyGameCharacter)
	{
		return;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = MyGameCharacter->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	// 이전에 왼클릭 한 시간 + 홀딩 시간 이 현재 시간보다 작아지면 충분히 누른것
	if (LastLeftMouseClickedTime + EnterHeavyAttackTime > GEngine->GetTimeSeconds())
	{
		return;
	}

	if (!bIsHeavyAttacking)
	{
		bIsHeavyAttacking = true;
		bIsBasicAttacking = false;
		AnimInstance->Montage_Play(AM_HeavyAttack_Press);
	}
	
	// TODO: 애니메이션 재생
	float IncreaseValue = bIsHeavyAttackGaugeIncrease ? GEngine->GetDeltaSeconds() : -GEngine->GetDeltaSeconds();

	CurrentChargeGauge += IncreaseValue;
	if (CurrentChargeGauge >= 1.0f)
	{
		CurrentChargeGauge = 1.0f;
		bIsHeavyAttackGaugeIncrease = false;
	}
	else if (CurrentChargeGauge <= 0.0f)
	{
		CurrentChargeGauge = 0.0f;
		bIsHeavyAttackGaugeIncrease = true;
	}

	if (const std::shared_ptr<USanhwaWidget>& SanhwaWidget = std::static_pointer_cast<USanhwaWidget>(MyGameCharacter->GetCharacterWidget()))
	{
		if (const std::shared_ptr<FProgressBarWidget>& PB_ChargeGauge = SanhwaWidget->GetChargeProgressBar())
		{
			PB_ChargeGauge->SetVisibility(true);
			PB_ChargeGauge->SetValue(CurrentChargeGauge);
		}
	}
	
}

void USanhwaCombatComponent::HeavyAttackMouseReleased()
{
	if (!MyGameCharacter)
	{
		return;
	}

	if (!bIsHeavyAttacking)
	{
		return;
	}

	const std::shared_ptr<UAnimInstance>& AnimInstance = MyGameCharacter->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	float Gap = 1.0f / GaugeSize;
	UINT CurrentGaugeIndex = std::min(static_cast<UINT>(CurrentChargeGauge / Gap), GaugeSize-1);
	if (SanhwaGauge[CurrentGaugeIndex])
	{
		// 강공격에 성공함
		AnimInstance->Montage_Play(AM_HeavyAttack_Attack);

		for (UINT i = 0; i < GaugeSize; ++i)
		{
			SanhwaGauge[i] = false;
		}
	}
	else
	{
		AnimInstance->Montage_Play(AM_HeavyAttack_Release);
	}

	bIsHeavyAttacking = false;
	bIsBasicAttacking = false;
	CurrentChargeGauge = 0.0f;
	

	if (const std::shared_ptr<USanhwaWidget>& SanhwaWidget = std::static_pointer_cast<USanhwaWidget>(MyGameCharacter->GetCharacterWidget()))
	{
		SanhwaWidget->SetGaugeUI(SanhwaGauge);
		if (const std::shared_ptr<FProgressBarWidget>& PB_ChargeGauge = SanhwaWidget->GetChargeProgressBar())
		{
			PB_ChargeGauge->SetVisibility(false);
		}
	}

	
}

void USanhwaCombatComponent::Attack4Success()
{
	// 비어있는 게이지 칸 탐색
	std::vector<int> EmptyGauge;
	EmptyGauge.reserve(GaugeSize);
	for (size_t i = 0; i < SanhwaGauge.size(); ++i)
	{
		if (!SanhwaGauge[i])
		{
			EmptyGauge.emplace_back(static_cast<int>(i));
		}
	}

	// 비어있는 게이지가 없는것
	if (EmptyGauge.empty())
	{
		return;
	}

	int NewGaugeIndex = MyMath::RandVector(EmptyGauge);
	SanhwaGauge[NewGaugeIndex] = true;

	if (MyGameCharacter)
	{
		if (const std::shared_ptr<USanhwaWidget>& SanhwaWidget = std::static_pointer_cast<USanhwaWidget>(MyGameCharacter->GetCharacterWidget()))
		{
			SanhwaWidget->SetGaugeUI(SanhwaGauge);
		}
	}
}

