#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class USanhwaAnimInstance : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(USanhwaAnimInstance)

	USanhwaAnimInstance();
	void UpdateAnimation(float dt) override;

	bool IsAllResourceOK() override;

	std::shared_ptr<UAnimSequence> AS_Test[5];
};