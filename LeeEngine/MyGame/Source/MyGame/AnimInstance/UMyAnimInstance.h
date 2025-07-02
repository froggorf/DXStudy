#pragma once
// 테스트용 AnimInstance

#include "Engine/Animation/UBlendSpace.h"
#include "../Component/UTestComponent.h"
#include "Engine/Animation/UAimOffsetBlendSpace.h"
#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Misc/Delegate.h"

class UMyAnimInstance : public UAnimInstance
{
	MY_GENERATE_BODY(UMyAnimInstance)
	static UMyAnimInstance* MyAnimInstance;

	UMyAnimInstance();

	~UMyAnimInstance() override
	{
	};

	void BeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	void UpdateAnimation(float dt) override;

	void AnimNotify_AttackStart();
	void AnimNotify_AttackEnd();
private:
	std::shared_ptr<UBlendSpace>    BS_MyUEFN_Locomotion;
	std::shared_ptr<UTestComponent> TestComp;


	std::shared_ptr<UAnimSequence> AS_Test0;
	std::shared_ptr<UAnimSequence> AS_Test1;
	std::shared_ptr<UAnimSequence> AS_Test2;

	std::shared_ptr<UAnimMontage> AM_Test;

	float CurrentSpeed = 0.0f;
};
