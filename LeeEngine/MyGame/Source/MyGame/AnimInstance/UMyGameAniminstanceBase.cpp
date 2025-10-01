#include "CoreMinimal.h"
#include "UMyGameAniminstanceBase.h"

#include "../Character/AMyGameCharacterBase.h"
#include "Engine/Components/USkeletalMeshComponent.h"

UMyGameAnimInstanceBase::UMyGameAnimInstanceBase()
{
}

void UMyGameAnimInstanceBase::LoadData_OnRegister()
{
	AssetManager::GetAsyncAssetCache(BS_LocomotionName, [this](std::shared_ptr<UObject> Object)
		{
			BS_Locomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});
}

void UMyGameAnimInstanceBase::Register()
{
	UAnimInstance::Register();

	LoadData_OnRegister();

}

void UMyGameAnimInstanceBase::BeginPlay()
{
	UAnimInstance::BeginPlay();

	MyGameCharacter = dynamic_cast<AMyGameCharacterBase*>(OwnerCharacter);
}

void UMyGameAnimInstanceBase::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();

	if (AActor* OwnerActor = GetSkeletalMeshComponent()->GetOwner())
	{
		if (AMyGameCharacterBase* MyGameCharacter = dynamic_cast<AMyGameCharacterBase*>(OwnerActor))
		{
			MovementComp = MyGameCharacter->GetCharacterMovement();
		}
	}
}

void UMyGameAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

}

void UMyGameAnimInstanceBase::EndMotionWarping()
{
	bUseMotionWarping = false;

	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	MotionWarpingComp->SetMotionWarping(false);
}

bool UMyGameAnimInstanceBase::SetMotionWarping()
{
	if (!OwnerCharacter)
	{
		return false;
	}

	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	if (!MotionWarpingComp)
	{
		return false;
	}


	MotionWarpingComp->SetMotionWarping(true);
	bUseMotionWarping = true;

	return true;
}

bool UMyGameAnimInstanceBase::IsAllResourceOK()
{
	return UAnimInstance::IsAllResourceOK() && MovementComp && BS_Locomotion ;
}


void UMyGameAnimInstanceBase::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);

	if (!IsAllResourceOK())
	{
		return;
	}

	// Locomotion Transition
	{
		XMFLOAT3 MoveVel = MovementComp->Velocity;

		MoveVel.y = 0.0f;
		float CurSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel)));
		BS_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, CurSpeed }, CurrentTime, FinalBoneMatrices, FinalNotifies);      
	}

	// spine_01 을 기준으로 상체와 하체를 블렌딩
	//LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, FinalBoneMatrices);

	// 몽타쥬 연결
	{
		std::string SlotName = "DefaultSlot";
		PlayMontage(SlotName, FinalBoneMatrices, FinalNotifies);
	}
}

