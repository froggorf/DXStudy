#include "CoreMinimal.h"
#include "UGideonCombatComponent.h"

#include "Engine/Class/Camera/UCameraComponent.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

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
	if (!static_cast<AGideonCharacter*>(MyGameCharacter)->IsAimMode())
	{
		return false;
	}

	if (!URangeBaseComponent::HeavyAttack())
	{
		return false;
	}



	HeavyAttackChargeTime += GEngine->GetDeltaSeconds();


	return true;
}

void UGideonCombatComponent::HeavyAttackMouseReleased()
{
	HeavyAttackChargeTime = 0.0f;

	URangeBaseComponent::HeavyAttackMouseReleased();
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
