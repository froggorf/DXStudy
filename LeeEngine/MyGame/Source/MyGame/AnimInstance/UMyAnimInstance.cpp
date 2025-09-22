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
			BS_LowPoly_Locomotion = std::dynamic_pointer_cast<UBlendSpace>(Object);
		});
	AssetManager::GetAsyncAssetCache("AS_LowPoly_Idle", [this](std::shared_ptr<UObject> Object)
		{
			AS_LowPoly_Idle = std::dynamic_pointer_cast<UAnimSequence>(Object);
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

	if (GetSkeletalMeshComponent() && TestComp&& BS_LowPoly_Locomotion && MovementComp)
	{
        // Locomotion Transition
        {
            XMFLOAT3 MoveVel = MovementComp->Velocity;
            
            MoveVel.y = 0.0f;
            float CurSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel)));
            
            BS_LowPoly_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, CurSpeed }, CurrentTime, FinalBoneMatrices, FinalNotifies);      
            
        }
		
		// spine_01 을 기준으로 상체와 하체를 블렌딩
		//LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, FinalBoneMatrices);

		// 몽타쥬 연결
		{
			std::string SlotName = "DefaultSlot";
			PlayMontage(SlotName, FinalBoneMatrices, FinalNotifies);
		}
	}
}
/*
 *XMFLOAT3 MoveVel = MovementComp->Velocity;
            MoveVel.y = 0.0f;
            float CurSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel)));
            EAnimState DesiredState = (CurSpeed > 5.0f) ? EAnimState::WalkRun : EAnimState::Idle;

            // 상태 변화 감지 및 블렌딩 시작
            if (CurrentState != DesiredState && (!LocomotionTransition.bBlending || CurDesiredState != DesiredState))
            {
                CurDesiredState = DesiredState;
                LocomotionTransition.bBlending = true;
                LocomotionTransition.BlendTime = 0.0f;
                NextState = DesiredState;
            }

            switch (CurrentState)
            {
            case EAnimState::Idle:
            {
                bool bDummy;
                AS_LowPoly_Idle->GetBoneTransform(CurrentTime, LocomotionTransition.PrevBoneMatrices, &bDummy);    
            }
            break;
            case EAnimState::WalkRun:
                BS_LowPoly_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, CurSpeed }, CurrentTime, LocomotionTransition.PrevBoneMatrices, FinalNotifies);        
                break;
            default:
                break;
            }

            if (LocomotionTransition.bBlending)
            {
                // 목표 상태의 본 행렬 계산
                std::vector<XMMATRIX> NewBoneMatrices(MAX_BONES, XMMatrixIdentity());
                switch (NextState)
                {
                case EAnimState::Idle:
                {
                    bool bDummy;
                    AS_LowPoly_Idle->GetBoneTransform(CurrentTime, NewBoneMatrices, &bDummy);    
                }
                break;
                case EAnimState::WalkRun:
                    BS_LowPoly_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f, CurSpeed }, CurrentTime, NewBoneMatrices, FinalNotifies);        
                    break;
                default:
                    break;
                }

                LocomotionTransition.BlendTime += static_cast<float>(1)/30.0f;
                float Alpha = LocomotionTransition.BlendTime / 0.25f;
                if (Alpha >= 1.0f)
                {
                    // 블렌딩 종료
                    LocomotionTransition.bBlending = false;
                    CurrentState = NextState;
                    FinalBoneMatrices = NewBoneMatrices;
                }
                else
                {
                    for (int i = 0; i < MAX_BONES; ++i)
                    {
                        FinalBoneMatrices[i] = LinearMatrixLerp(LocomotionTransition.PrevBoneMatrices[i], NewBoneMatrices[i], std::min(Alpha+0.1f, 1.0f) );
                    }
                }
            }
            else
            {
                FinalBoneMatrices = LocomotionTransition.PrevBoneMatrices;
            }
 */


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

