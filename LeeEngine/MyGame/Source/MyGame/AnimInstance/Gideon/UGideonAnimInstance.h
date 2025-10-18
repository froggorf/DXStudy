#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class UGideonAnimInstance final : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(UGideonAnimInstance)

	UGideonAnimInstance();

	void LoadData_OnRegister() override;
	void SetAnimNotify_BeginPlay() override;

	bool IsAllResourceOK() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateAnimation(float dt) override;


	//void MotionWarping_HeavyAttack_Move();
	//void MotionWarping_HeavyAttack_Stay();

	//void Ultimate_ChangeCameraToNormal();

	void BasicAttack0();
	void BasicAttack1();
	void BasicAttack2();
	//void HeavyAttack();
	//void SkillAttack();
	//void UltAttack();

	// MotionWarping - SkillAttack
	//void SanhwaSkillAttack();

	//void SetSkeletalMeshVisibility(bool NewVisibility);

	//std::shared_ptr<UAnimSequence> AS_Test[5];

private:
	std::shared_ptr<UBlendSpace> BS_FPSLocomotion;
	float Direction = 0.0f;
};