// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "UMeleeBaseComponent.h"

class USanhwaCombatComponent : public UMeleeBaseComponent
{
	MY_GENERATE_BODY(USanhwaCombatComponent)
	USanhwaCombatComponent();
	~USanhwaCombatComponent() override = default;
public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

	void BasicAttack() override;
	void StrongBasicAttack() override {};
	void JumpAttack() override {};
};
