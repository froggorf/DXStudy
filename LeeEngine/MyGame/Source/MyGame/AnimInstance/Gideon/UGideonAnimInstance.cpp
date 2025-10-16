#include "CoreMinimal.h"
#include "UGideonAnimInstance.h"

#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Component/Combat/Range/UGideonCombatComponent.h"

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

void UGideonAnimInstance::BasicAttack0()
{
	if (!MyGameCharacter)
	{
		return;
	}
	std::shared_ptr<UGideonCombatComponent> GideonCombatComp = std::dynamic_pointer_cast<UGideonCombatComponent>(MyGameCharacter->GetCombatComponent());
	if (!GideonCombatComp)
	{
		return;
	}

	GideonCombatComp->ApplyBasicAttack0();
}

void UGideonAnimInstance::SetAnimNotify_BeginPlay()
{
	UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay();

	Delegate<> Attack0Delegate;
	Attack0Delegate.Add(this, &UGideonAnimInstance::BasicAttack0);
	NotifyEvent["BasicAttack0"] = Attack0Delegate;


}
