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

void UGideonAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);

	// 몽타쥬 연결
	{
		std::vector<FBoneLocalTransform> UpperBodyBoneTransforms = GetInitialLocalBoneTransforms();
		if (PlayMontage("UpperBody", UpperBodyBoneTransforms, FinalNotifies))
		{
			LayeredBlendPerBone(BoneTransforms, UpperBodyBoneTransforms, "spine_01", 1.0f, BoneTransforms);	
		}
	}

}

void UGideonAnimInstance::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();

	// 예시)
	//	Delegate<> Attack0Delegate;
	//	Attack0Delegate.Add(this, &USanhwaAnimInstance::MotionWarping_BasicAttack0);
	//	NotifyEvent["SH_Attack0"] = Attack0Delegate;
}
