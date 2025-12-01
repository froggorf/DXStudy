#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class UEnemyAnimInstanceBase : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(UEnemyAnimInstanceBase)

	UEnemyAnimInstanceBase() = default;

	void LoadData_OnRegister() override;
	void SetAnimNotify_BeginPlay() override;
	void BeginPlay() override;

	bool IsAllResourceOK() override;

	void DoAttackAnim(const Delegate<>& OnFinishAttack);
	void ApplyBasicAttack();
protected:
	std::string AttackMontageName;
	std::shared_ptr<UAnimMontage> AM_Attack;

	class AEnemyBase* OwnerEnemy;
};

class UWolfAnimInstance final : public UEnemyAnimInstanceBase
{
	MY_GENERATE_BODY(UWolfAnimInstance)

	UWolfAnimInstance();
	~UWolfAnimInstance() override = default;
};

class UPigAnimInstance final : public UEnemyAnimInstanceBase
{
	MY_GENERATE_BODY(UPigAnimInstance)

	UPigAnimInstance();
	~UPigAnimInstance() override = default;
};

class UDragonAnimInstance final : public UEnemyAnimInstanceBase
{
	MY_GENERATE_BODY(UDragonAnimInstance)

	UDragonAnimInstance();
	~UDragonAnimInstance() override = default;
};