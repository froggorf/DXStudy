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
	bool HeavyAttack() override;
	void HeavyAttackMouseReleased() override;
	void JumpAttack() override {};

	float GetAttack4_FloatingDistance() const {return Attack4_FloatingDistance;}
	float GetAttack4_FloatMoveDistance() const {return Attack4_FloatMoveDistance; }

	void Attack4Success();
	float GetHeavyAttackMoveDistance() const {return HeavyAttackMoveDistance;}
private:
	// ================ 기본공격 변수 ================
	// 기본공격 4의 떠오르는 높이
	float Attack4_FloatingDistance = 500.0f;
	// 기본공격 4의 떠오르는 중의 움직일 수 있는 거리
	float Attack4_FloatMoveDistance = 300.0f;

	// ================ 강공격 변수 ================
	bool bIsHeavyAttackGaugeIncrease = true;
	float CurrentChargeGauge = 0.0f;
	float HeavyAttackMoveDistance = 600.0f;
	// 강공격 애니메이션들
	std::shared_ptr<UAnimMontage> AM_HeavyAttack_Press;
	std::shared_ptr<UAnimMontage> AM_HeavyAttack_Release;
	
public:
	// 산화의 특수 게이지
	static constexpr UINT GaugeSize = 6;
private:
	std::array<bool, GaugeSize> SanhwaGauge = {false, };
};
