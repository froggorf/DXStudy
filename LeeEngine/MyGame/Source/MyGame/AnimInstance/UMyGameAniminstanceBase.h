#pragma once
#include "Engine/Animation/UBlendSpace.h"
#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Misc/Delegate.h"

class UCharacterMovementComponent;


class UMyGameAnimInstanceBase : public UAnimInstance
{
	MY_GENERATE_BODY(UMyGameAnimInstanceBase)

	UMyGameAnimInstanceBase();
	~UMyGameAnimInstanceBase() override = default;

	void BeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateAnimation(float dt) override;
	
	bool IsAllResourceOK() override;

private:
	UCharacterMovementComponent* MovementComp;

	std::shared_ptr<UBlendSpace> BS_Locomotion;
};
