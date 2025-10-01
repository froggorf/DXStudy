#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class USanhwaAnimInstance : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(USanhwaAnimInstance)

	USanhwaAnimInstance();
	void BeginPlay() override;
	void UpdateAnimation(float dt) override;

	bool IsAllResourceOK() override;

	void MotionWarping_BasicAttack0();
	void MotionWarping_BasicAttack1();
	void MotionWarping_BasicAttack2();
	void MotionWarping_BasicAttack3();
	void MotionWarping_BasicAttack4();

	std::shared_ptr<UAnimSequence> AS_Test[5];
};