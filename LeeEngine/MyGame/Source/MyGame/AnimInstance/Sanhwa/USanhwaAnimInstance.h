#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class USanhwaAnimInstance final : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(USanhwaAnimInstance)

	USanhwaAnimInstance();

	void SetAnimNotify_BeginPlay() override;
	void BeginPlay() override;
	void UpdateAnimation(float dt) override;

	bool IsAllResourceOK() override;

	// MotionWarping - BasicAttack
	void MotionWarping_BasicAttack0();
	void MotionWarping_BasicAttack1();
	void MotionWarping_BasicAttack2();
	void MotionWarping_BasicAttack3();
	void MotionWarping_BasicAttack4_Float();
	void MotionWarping_BasicAttack4_Floating();
	void MotionWarping_BasicAttack4_Attack();

	void MotionWarping_HeavyAttack_Move();
	void MotionWarping_HeavyAttack_Stay();

	void Ultimate_ChangeCameraToNormal();

	void Attack4();
	void SkillAttack();
	void UltAttack();

	// MotionWarping - SkillAttack
	void SanhwaSkillAttack();

	void SetSkeletalMeshVisibility(bool NewVisibility);

	std::shared_ptr<UAnimSequence> AS_Test[5];

private:
	XMFLOAT3 Attack4_AttackTargetPos;
};