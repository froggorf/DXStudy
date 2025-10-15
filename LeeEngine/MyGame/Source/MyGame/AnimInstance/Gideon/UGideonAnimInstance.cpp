#include "CoreMinimal.h"
#include "UGideonAnimInstance.h"

UGideonAnimInstance::UGideonAnimInstance()
{
	BS_LocomotionName = "BS_Gideon_Locomotion";
}

bool UGideonAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK();
}

void UGideonAnimInstance::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();

	// 예시)
	//	Delegate<> Attack0Delegate;
	//	Attack0Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack0);
	//	NotifyEvent["SH_Attack0"] = Attack0Delegate;
}