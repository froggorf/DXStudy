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
	AssetManager::GetAsyncAssetCache("BS_MyUEFN_LocomotionRPG", [this](std::shared_ptr<UObject> Object)
		{
			BS_MyUEFN_Locomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});

	AssetManager::GetAsyncAssetCache("AS_MyUEFN_Idle", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test0 = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});
	AssetManager::GetAsyncAssetCache("AS_Aim", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test1 = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});
	AssetManager::GetAsyncAssetCache("AS_Pistol", [this](std::shared_ptr<UObject> Object)
		{
			AS_Test2 = std::dynamic_pointer_cast<UAnimSequence>(Object);
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

	if (GetSkeletalMeshComponent() && TestComp&& BS_MyUEFN_Locomotion  && AS_Test0 && AS_Test1 && AS_Test2 && MovementComp)
	{
		// BlendSpace_Locomotion
		std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES, XMMatrixIdentity());
		XMFLOAT3 MoveVel = MovementComp->Velocity;
		MoveVel.y = 0.0f;
		BS_MyUEFN_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel))) }, CurrentTime, BS_IdleWalkRunMatrices, FinalNotifies);

		// 애니메이션 시퀀스 계산
		std::vector<XMMATRIX> AS_Matrices(MAX_BONES, XMMatrixIdentity());
		switch (TestComp->TargetAnim)
		{
		case 0:
			AS_Test0->GetBoneTransform(CurrentTime, AS_Matrices, &bPlayRootMotion);	
			break;
		case 1:
			AS_Test1->GetBoneTransform(CurrentTime, AS_Matrices, &bPlayRootMotion);
			break;
		case 2:
			AS_Test2->GetBoneTransform(CurrentTime, AS_Matrices, &bPlayRootMotion);
			break;
		default:
			break;
		}
		
		// spine_01 을 기준으로 상체와 하체를 블렌딩
		LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, FinalBoneMatrices);

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

