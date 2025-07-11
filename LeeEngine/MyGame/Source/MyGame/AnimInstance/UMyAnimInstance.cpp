#include "CoreMinimal.h"
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "../Actor/ATestCube2.h"
#include "../Actor/ATestPawn.h"
#include "Engine/Components/USkeletalMeshComponent.h"

UMyAnimInstance* UMyAnimInstance::MyAnimInstance;

UMyAnimInstance::UMyAnimInstance()
{
	AssetManager::GetAsyncAssetCache("BS_LowPoly_LocomotionRPG", [this](std::shared_ptr<UObject> Object)
		{
			BS_MyUEFN_Locomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});

}

void UMyAnimInstance::BeginPlay()
{
	UAnimInstance::BeginPlay();

	NotifyEvent["AttackStart"] = Delegate<>{};
	NotifyEvent["AttackStart"].Add(this, &UMyAnimInstance::AnimNotify_AttackStart);

	NotifyEvent["AttackEnd"] = Delegate<>{};
	NotifyEvent["AttackEnd"].Add(this, &UMyAnimInstance::AnimNotify_AttackEnd);
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();

	if (AActor* OwnerActor = GetSkeletalMeshComponent()->GetOwner())
	{
		if (auto TestActor = dynamic_cast<ATestPawn*>(OwnerActor))
		{
			if (const std::shared_ptr<UTestComponent>& OwnerTestComp = std::dynamic_pointer_cast<UTestComponent>(TestActor->FindComponentByClass("UTestComponent")))
			{
				TestComp = OwnerTestComp;
			}
			MovementComp = TestActor->GetCharacterMovement();
		}
	}
	MyAnimInstance = this;
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

}

void UMyAnimInstance::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);

	if (GetSkeletalMeshComponent() && TestComp&& BS_MyUEFN_Locomotion && MovementComp)
	{
		// BlendSpace_Locomotion
		//std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES, XMMatrixIdentity());
		XMFLOAT3 MoveVel = MovementComp->Velocity;
		MoveVel.y = 0.0f;
		BS_MyUEFN_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel))) }, CurrentTime, FinalBoneMatrices, FinalNotifies);
		
		// spine_01 을 기준으로 상체와 하체를 블렌딩
		//LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, FinalBoneMatrices);

		// 몽타쥬 연결
		{
			std::string SlotName = "DefaultSlot";
			PlayMontage(SlotName, FinalBoneMatrices, FinalNotifies);
		}
	}
}


void UMyAnimInstance::AnimNotify_AttackStart()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMeshComponent())
	{
		if (AActor* Actor = SkeletalMesh->GetOwner())
		{
			if (ATestPawn* MyPawn = dynamic_cast<ATestPawn*>(Actor))
			{
				MyPawn->SetAttackStart();
			}
		}
	}
}

void UMyAnimInstance::AnimNotify_AttackEnd()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetSkeletalMeshComponent())
	{
		if (AActor* Actor = SkeletalMesh->GetOwner())
		{
			if (ATestPawn* MyPawn = dynamic_cast<ATestPawn*>(Actor))
			{
				MyPawn->SetAttackEnd();
			}
		}
	}
}

