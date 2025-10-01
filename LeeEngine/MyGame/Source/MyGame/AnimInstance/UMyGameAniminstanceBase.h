#pragma once
#include "Engine/Animation/UBlendSpace.h"
#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Misc/Delegate.h"

class AMyGameCharacterBase;
class UCharacterMovementComponent;


class UMyGameAnimInstanceBase : public UAnimInstance
{
	MY_GENERATE_BODY(UMyGameAnimInstanceBase)

	UMyGameAnimInstanceBase();
	~UMyGameAnimInstanceBase() override = default;

	virtual void LoadData_OnRegister();
	void Register() override;
	void BeginPlay() override;
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateAnimation(float dt) override;

	void EndMotionWarping();
	bool SetMotionWarping();

	bool IsAllResourceOK() override;

protected:
	std::string BS_LocomotionName = "BS_UE4_Locomotion";

protected:
	AMyGameCharacterBase* MyGameCharacter = nullptr;
private:
	UCharacterMovementComponent* MovementComp;

	std::shared_ptr<UBlendSpace> BS_Locomotion;
};
