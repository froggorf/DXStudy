#include "CoreMinimal.h"
#include "UGideonCombatComponent.h"

#include "Engine/Class/Camera/UCameraComponent.h"
#include "MyGame/AnimInstance/Gideon/UGideonAnimInstance.h"
#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Widget/Gideon/UGideonWidget.h"

UGideonCombatComponent::UGideonCombatComponent()
{
}

void UGideonCombatComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	URangeBaseComponent::Initialize(MyCharacter);

	SetBasicAttackData({
		"AM_Gideon_BasicAttack0",
		"AM_Gideon_BasicAttack1",
		"AM_Gideon_BasicAttack2",
	},
	{
		FAttackData{{10,10,10}, 1.0f, 0.0f, 10.0f, true},
		FAttackData{{20,20,20}, 1.25f, 0.0f, 10.0f, true},
		FAttackData{{30,30,30}, 1.5f, 0.0f, 15.0f, true},
	});

	AssetManager::GetAsyncAssetCache("AM_Gideon_Charge",[this](std::shared_ptr<UObject> Object)
		{
			HeavyAttack_ChargeMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!HeavyAttack_ChargeMontage)
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, ", HeavyAttack_ChargeMontage Montage Not exist");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist HeavyAttack_ChargeMontage");
#endif
			}
		});

	AssetManager::GetAsyncAssetCache("AM_Gideon_HeavyAttack",[this](std::shared_ptr<UObject> Object)
		{
			HeavyAttack_AttackMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
			if (!HeavyAttack_AttackMontage)
			{
				MY_LOG("Warning", EDebugLogLevel::DLL_Error, ", HeavyAttack_AttackMontage Montage Not exist");
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
				// 개발 중 테스트를 위하여 assert
				assert(nullptr&&"Not exist HeavyAttack_AttackMontage");
#endif
			}
		}); 
}

void UGideonCombatComponent::BasicAttack()
{
	if (!static_cast<AGideonCharacter*>(MyGameCharacter)->IsAimMode())
	{
		return;
	}

	URangeBaseComponent::BasicAttack();
}

bool UGideonCombatComponent::HeavyAttack()
{
	if (!HeavyAttack_AttackMontage || !HeavyAttack_ChargeMontage)
	{
		return false;
	}
	AGideonCharacter* GideonCharacter = dynamic_cast<AGideonCharacter*>(MyGameCharacter);
	if (!GideonCharacter || !GideonCharacter->IsAimMode())
	{
		return false;
	}

	if (!URangeBaseComponent::HeavyAttack())
	{
		return false;
	}

	if (!bIsHeavyAttacking)
	{
		bIsHeavyAttacking = true;
		if (const std::shared_ptr<UGideonAnimInstance>& GideonAnimInstance = std::dynamic_pointer_cast<UGideonAnimInstance>(MyGameCharacter->GetAnimInstance()))
		{
			GideonAnimInstance->Montage_Play(HeavyAttack_ChargeMontage);
		}
		
	}

	SetHeavyAttackChargeTime(HeavyAttackChargeTime + GEngine->GetDeltaSeconds());

	return true;
}

void UGideonCombatComponent::HeavyAttackMouseReleased()
{

	URangeBaseComponent::HeavyAttackMouseReleased();

	if (HeavyAttackChargeTime > 0.1f && HeavyAttack_AttackMontage)
	{ 
		if (const std::shared_ptr<UGideonAnimInstance>& GideonAnimInstance = std::dynamic_pointer_cast<UGideonAnimInstance>(MyGameCharacter->GetAnimInstance()))
		{
			GideonAnimInstance->Montage_Play(HeavyAttack_AttackMontage);
		}
	}

	SetHeavyAttackChargeTime(0);

}

void UGideonCombatComponent::ApplyBasicAttack(const std::string& SpawnSocketName, size_t AttackIndex)
{
	if (AGideonCharacter* GideonCharacter = dynamic_cast<AGideonCharacter*>(MyGameCharacter))
	{
		if (!GideonCharacter->IsAimMode())
		{
			return;
		}

		USkeletalMeshComponent* GideonSkeletalMesh = GideonCharacter->GetSkeletalMeshComponent();
		if (!GideonSkeletalMesh)
		{
			return;
		}

		const std::shared_ptr<UCameraComponent>& AimModeCameraComp = GideonCharacter->GetAimModeCameraComp();
		const XMFLOAT3 Start = AimModeCameraComp->GetWorldLocation();
		const XMFLOAT3 Forward = AimModeCameraComp->GetForwardVector();
		const XMFLOAT3 End = Start + Forward * FireBallThrowDistance;

		FTransform SpawnTransform = GideonSkeletalMesh->GetSocketTransform(SpawnSocketName);
		SpawnTransform.Scale3D = XMFLOAT3{1,1,1};
		SpawnTransform.Rotation = XMFLOAT4{0,0,0,1};
		std::vector<ECollisionChannel> Channels;
		for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
		{
			Channels.emplace_back(static_cast<ECollisionChannel>(i));
		}

		FHitResult Result;
		XMFLOAT3 TargetPosition = End;
		if (GPhysicsEngine->LineTraceSingleByChannel(Start,End, Channels, Result, 3.0f))
		{
			// 부딪히는 곳이 있다면 그 위치로 던짐
			TargetPosition = Result.Location;
		}
		GideonCharacter->SpawnFireBall(SpawnTransform, GetBasicAttackData(AttackIndex), TargetPosition);
	}
}

void UGideonCombatComponent::ApplyBasicAttack0()
{
	ApplyBasicAttack("hand_r", 0);
}

void UGideonCombatComponent::ApplyBasicAttack1()
{
	ApplyBasicAttack("hand_l", 1);
}

void UGideonCombatComponent::ApplyBasicAttack2()
{
	ApplyBasicAttack("hand_r", 2);
}

void UGideonCombatComponent::SetHeavyAttackChargeTime(float NewValue)
{
	HeavyAttackChargeTime = std::min(NewValue,MaxHeavyAttackChargeTime);
	if (const std::shared_ptr<UGideonWidget>& GideonWidget = std::static_pointer_cast<UGideonWidget>(MyGameCharacter->GetCharacterWidget()))
	{
		GideonWidget->SetLightningAttackChargeGauge(HeavyAttackChargeTime, MaxHeavyAttackChargeTime);
	}

}
