// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Timer/FTimerManager.h"


class AMyGameCharacterBase;
class UPlayerInput;
class AMyGameCharacterBase;

class UCombatBaseComponent : public UActorComponent
{
	MY_GENERATE_BODY(UCombatBaseComponent)
	UCombatBaseComponent();

public:
	void BeginPlay() override;

	// AMyGameCharacterBase 내에서 Register 에 호출됨
	// 초기 세팅
	virtual void Initialize(AMyGameCharacterBase* MyCharacter);

	virtual void BasicAttack();
	virtual void BasicAttackEnded();
	virtual void StrongBasicAttack() {};
	virtual void JumpAttack() {};
	virtual void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem);
protected:
	// Initialize 안에서 적용
	void SetBasicAttackMontages(const std::vector<std::string>& NewMontagesName);

protected:
	AMyGameCharacterBase* OwnerCharacter = nullptr;

public:
	void SetFightMode(bool NewMode);
	void FindNearbyEnemy();

protected:
	// 초기 세팅을 위해
	bool bIsFightMode = true;
	FTimerHandle FindEnemyHandle;
	const float EnemyFindTime_OnFight = 0.25f;
	const float EnemyFindTime_NoFight = 1.0f;
	const float EnemyFindRadius = 50000.0f;

	// ===================== Basic Attack =====================
	// BasicAttackMontages.size() 가 기본공격의 콤보 수
	std::vector<std::shared_ptr<UAnimMontage>> BasicAttackMontages;
	// 선입력을 위해 키입력 시간을 저장
	float LastBasicAttackClickedTime = -1.0f;
	// 선입력 시간 : 0.5f
	float BasicAttackApplyTime = 0.5f;
	// 현재의 기본공격 단계를 저장
	UINT CurrentBasicAttackCombo = 0;
	bool bIsBasicAttacking = false;



};
