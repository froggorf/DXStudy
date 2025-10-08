// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "UMeleeBaseComponent.h"

class USanhwaCombatComponent final : public UMeleeBaseComponent
{
	MY_GENERATE_BODY(USanhwaCombatComponent)
	USanhwaCombatComponent();
	~USanhwaCombatComponent() override = default;
public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

	void BasicAttack() override;
	void StrongBasicAttack() override {};
	void JumpAttack() override {};

	float GetAttack4_FloatingDistance() const {return Attack4_FloatingDistance;}
	float GetAttack4_FloatMoveDistance() const {return Attack4_FloatMoveDistance; }
	float GetAttack4_AttackMoveDistance() const {return Attack4_AttackMoveDistance; }

	void Attack4Success();
private:
	// 기본공격 4의 떠오르는 높이
	float Attack4_FloatingDistance = 500.0f;
	// 기본공격 4의 떠오르는 중의 움직일 수 있는 거리
	float Attack4_FloatMoveDistance = 300.0f;
	// 기본공격 4의 공격 중의 움직일 수 있는 거리
	float Attack4_AttackMoveDistance = 700.0f;

	
public:
	// 산화의 특수 게이지
	static constexpr UINT GaugeSize = 6;
private:
	std::array<bool, GaugeSize> SanhwaGauge = {false, };
};
