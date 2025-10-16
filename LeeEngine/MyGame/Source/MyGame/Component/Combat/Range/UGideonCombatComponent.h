// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "URangeBaseComponent.h"

class UGideonCombatComponent final : public URangeBaseComponent
{
	MY_GENERATE_BODY(UGideonCombatComponent)
	UGideonCombatComponent();
	~UGideonCombatComponent() override = default;
public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

	void BasicAttack() override;
	bool HeavyAttack() override;
	void HeavyAttackMouseReleased() override;
	void JumpAttack() override {};


	void ApplyBasicAttack0();
private:
public:
private:
	static constexpr float FireBallThrowDistance = 1000.0f;
};
 