#include "CoreMinimal.h"
#include "UEnemyAnimInstance.h"

#include "Engine/AssetManager/AssetManager.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"

void UEnemyAnimInstanceBase::LoadData_OnRegister()
{
	UMyGameAnimInstanceBase::LoadData_OnRegister();

	// Attack Montage Load
	AssetManager::GetAsyncAssetCache(AttackMontageName, [this](std::shared_ptr<UObject> Object)
		{
			AM_Attack = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});

}


bool UEnemyAnimInstanceBase::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK() && AM_Attack;
}

void UEnemyAnimInstanceBase::DoAttackAnim(const Delegate<>& OnFinishAttack)
{
	Montage_Play(AM_Attack, 0, OnFinishAttack);
}

void UEnemyAnimInstanceBase::ApplyBasicAttack()
{
	if (OwnerEnemy)
	{
		OwnerEnemy->ApplyBasicAttack();
	}
}

void UEnemyAnimInstanceBase::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();

	// TODO: 맞춰서 넣기
	//// BasicAttack
	NotifyEvent["BasicAttack"] = {this, &UEnemyAnimInstanceBase::ApplyBasicAttack};
}

void UEnemyAnimInstanceBase::BeginPlay()
{
	UMyGameAnimInstanceBase::BeginPlay();

	OwnerEnemy = dynamic_cast<AEnemyBase*>(OwnerCharacter);
}


UWolfAnimInstance::UWolfAnimInstance()
{
	BS_LocomotionName = "BS_Wolf_Locomotion";
	AttackMontageName = "AM_Wolf_Attack";
}

UPigAnimInstance::UPigAnimInstance()
{
	BS_LocomotionName = "BS_Pig_Locomotion";
	AttackMontageName = "AM_Pig_Attack";
}

UDragonAnimInstance::UDragonAnimInstance()
{
	BS_LocomotionName = "BS_Dragon_Locomotion";
	AttackMontageName = "AM_Dragon_Attack";
}
