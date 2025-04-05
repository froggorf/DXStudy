#pragma once
// 테스트용 AnimInstance

#include "Engine/Animation/UBlendSpace.h"
#include "../Component/UTestComponent.h"
#include "Engine/Animation/UAimOffsetBlendSpace.h"
#include "Engine/Animation/UAnimInstance.h"

class UMyAnimInstance : public UAnimInstance
{
	MY_GENERATED_BODY(UMyAnimInstance)
public:
	UMyAnimInstance();
	~UMyAnimInstance() override {};

	void BeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	void UpdateAnimation(float dt) override;
protected:
private:

public:
protected:
private:
	std::shared_ptr<UBlendSpace> BS_MyUEFN_Locomotion;
	std::shared_ptr<UTestComponent> TestComp;

	std::shared_ptr<UAimOffsetBlendSpace> AO_MyUEFN_Stand;


	float CurrentSpeed = 0.0f;

};