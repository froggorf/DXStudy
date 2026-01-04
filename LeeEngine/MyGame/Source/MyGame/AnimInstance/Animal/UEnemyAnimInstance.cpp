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

void UDragonAnimInstance::SetAnimNotify_BeginPlay()
{
	UEnemyAnimInstanceBase::SetAnimNotify_BeginPlay();

	NotifyEvent["FlameStart"] = {this, &UDragonAnimInstance::FlameStart};
	NotifyEvent["FlameEnd"] = {this, &UDragonAnimInstance::FlameEnd};

	NotifyEvent["HPFlameStart"] = {this, &UDragonAnimInstance::HPFlameStart};
	NotifyEvent["HPFlameEnd"] = {this, &UDragonAnimInstance::HPFlameEnd};
}

void UDragonAnimInstance::FlameStart()
{
	if (ADragon* Dragon = dynamic_cast<ADragon*>(OwnerCharacter))
	{
		Dragon->StartFlame();
		MY_LOG("Dragon", EDebugLogLevel::DLL_Warning, "Start Flame");
	}
}

void UDragonAnimInstance::FlameEnd()
{
	if (ADragon* Dragon = dynamic_cast<ADragon*>(OwnerCharacter))
	{
		Dragon->EndFlame();
		MY_LOG("Dragon", EDebugLogLevel::DLL_Warning, "End Flame");
	}
}

void UDragonAnimInstance::HPFlameStart()
{
	if (ADragon* Dragon = dynamic_cast<ADragon*>(OwnerCharacter))
	{
		Dragon->StartHPSkill();
		MY_LOG("Dragon", EDebugLogLevel::DLL_Warning, "Start Flame");
	}
}

void UDragonAnimInstance::HPFlameEnd()
{
	if (ADragon* Dragon = dynamic_cast<ADragon*>(OwnerCharacter))
	{
		Dragon->EndHPSkill();
		MY_LOG("Dragon", EDebugLogLevel::DLL_Warning, "End Flame");
	}
}
