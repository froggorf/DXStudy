#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class UGideonAnimInstance final : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(UGideonAnimInstance)

	UGideonAnimInstance();

	void SetAnimNotify_BeginPlay() override;

	bool IsAllResourceOK() override;

	void UpdateAnimation(float dt) override;

	//// MotionWarping - BasicAttack
	//void MotionWarping_BasicAttack0();
	//void MotionWarping_BasicAttack1();
	//void MotionWarping_BasicAttack2();
	//void MotionWarping_BasicAttack3();
	//void MotionWarping_BasicAttack4_Float();
	//void MotionWarping_BasicAttack4_Floating();
	//void MotionWarping_BasicAttack4_Attack();

	//void MotionWarping_HeavyAttack_Move();
	//void MotionWarping_HeavyAttack_Stay();

	//void Ultimate_ChangeCameraToNormal();

	void BasicAttack0();
	//void BasicAttack1();
	//void BasicAttack2();
	//void BasicAttack3();
	//void BasicAttack4();
	//void HeavyAttack();
	//void SkillAttack();
	//void UltAttack();

	// MotionWarping - SkillAttack
	//void SanhwaSkillAttack();

	//void SetSkeletalMeshVisibility(bool NewVisibility);

	//std::shared_ptr<UAnimSequence> AS_Test[5];

private:
	//XMFLOAT3 Attack4_AttackTargetPos;
};