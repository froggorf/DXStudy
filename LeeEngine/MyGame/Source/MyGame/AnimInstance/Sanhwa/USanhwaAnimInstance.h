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

	void BasicAttack0();
	void BasicAttack1();
	void BasicAttack2();
	void BasicAttack3();
	void BasicAttack4();
	void HeavyAttack();
	void SkillAttack();
	void UltAttack();

	// MotionWarping - SkillAttack
	void SanhwaSkillAttack();

	void               SetSkeletalMeshVisibility(bool NewVisibility);

	
	std::string GetJumpStartAnimSequenceName() override { return "AS_Sanhwa_JumpStart";};
	std::string GetFallingAnimSequenceName() override { return "AS_Sanhwa_Falling";};
	std::string GetLandAnimSequenceName() override { return "AS_Sanhwa_Landing";};
protected:
	float GetLandingTimeMultiply() override {return 20.0f;}
private:
	XMFLOAT3 Attack4_AttackTargetPos;
};