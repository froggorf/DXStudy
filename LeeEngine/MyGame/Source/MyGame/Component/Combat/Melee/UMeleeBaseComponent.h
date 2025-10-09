// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "../UCombatBaseComponent.h"
#include "Engine/Components/UAnimMontage.h"

class UMeleeBaseComponent : public UCombatBaseComponent
{
	MY_GENERATE_BODY(UMeleeBaseComponent)
	UMeleeBaseComponent();

public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

	void BasicAttack() override;
	void HeavyAttack() override;
	void JumpAttack() override {};


};
