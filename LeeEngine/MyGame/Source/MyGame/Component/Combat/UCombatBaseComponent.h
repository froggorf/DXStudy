// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "Engine/Components/UAnimMontage.h"


class UCombatBaseComponent : public UActorComponent
{
	MY_GENERATE_BODY(UCombatBaseComponent)
	UCombatBaseComponent();

public:
	virtual void Initialize() {};

	virtual void BasicAttack() {};
	virtual void StrongBasicAttack() {};
	virtual void Skill() {};
	virtual void Ultimate() {};
	virtual void JumpAttack() {};

public:
	void SetBasicAttackMontages(const std::vector<std::string>& NewMontagesName);
protected:
	// BasicAttackMontages.size() 가 기본공격의 콤보 수
	std::vector<std::shared_ptr<UAnimMontage>> BasicAttackMontages;
};
