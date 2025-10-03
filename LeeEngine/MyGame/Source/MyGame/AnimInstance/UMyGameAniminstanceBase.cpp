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

void UMyGameAnimInstanceBase::SetAnimNotify_BeginPlay()
{
	UAnimInstance::SetAnimNotify_BeginPlay();

	NotifyEvent["MotionWarpingEnd"] = Delegate<>{};
	NotifyEvent["MotionWarpingEnd"].Add(static_cast<UMyGameAnimInstanceBase*>(this), &UMyGameAnimInstanceBase::EndMotionWarping);
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

XMFLOAT3 UMyGameAnimInstanceBase::GetWarpingPositionToEnemy(const XMFLOAT3& CurActorLocation, XMFLOAT3& EnemyLocation, float MaxMoveDistance)
{
	// y좌표 이동을 제어하기위해
	EnemyLocation.y = CurActorLocation.y;

	float ToEnemyDistance = MyMath::GetDistance(CurActorLocation, EnemyLocation);
	XMFLOAT3 ToEnemyVector = MyMath::GetDirectionUnitVector(CurActorLocation, EnemyLocation);

	// 적군의 길이 30 -> 적의 위치까지 가려다가 충돌체크가 이상하게 되는 경우를 방지하기 위해 약간의 갭을 두고 이동
	// std::min(max(ToEnemyDistance - 30, 0), MaxMoveDistance) -> 이동할 수 있는 거리와 적군 앞까지의 거리중 작은것을 선택
	return CurActorLocation + ToEnemyVector * std::min(max(ToEnemyDistance - 30, 0), MaxMoveDistance);
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
		BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, CurSpeed }, CurrentTime, BoneTransforms, FinalNotifies);      
	}

	// spine_01 을 기준으로 상체와 하체를 블렌딩
	//LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, FinalBoneMatrices);

	// 몽타쥬 연결
	{
		PlayMontage("DefaultSlot", BoneTransforms, FinalNotifies);

		//std::vector<FBoneLocalTransform> MontageTransforms(MAX_BONES);
		//std::string SlotName = "DefaultSlot";
		//PlayMontage(SlotName, MontageTransforms, FinalNotifies);
		//std::vector<FBoneLocalTransform> CurrentBoneTransforms = BoneTransforms;
		//LayeredBlendPerBone(CurrentBoneTransforms, MontageTransforms, "spine_01", 1, BoneTransforms);
	}
}

void UMyGameAnimInstanceBase::SetWarping_BasicAttack(size_t BasicAttackComboIndex, const XMFLOAT3& IfNoEnemyWarpingDirectionUnitVector)
{
	if (!SetMotionWarping())
	{
		return;
	}
	const std::shared_ptr<UMotionWarpingComponent>& MotionWarpingComp = MyGameCharacter->GetMotionWarpingComponent();
	const std::shared_ptr<UCombatBaseComponent>& CombatComp = MyGameCharacter->GetCombatComponent();
	if (!MotionWarpingComp || !CombatComp)
	{
		return;
	}

	XMFLOAT3 CurActorLocation = MyGameCharacter->GetActorLocation();
	float MoveDistance = CombatComp->GetBasicAttackMoveDistance(BasicAttackComboIndex);
	float MoveTime = CombatComp->GetBasicAttackMontage(BasicAttackComboIndex)->GetPlayLength();

	XMFLOAT3 WarpingTargetPos;
	if (const AActor* NearestEnemy = CombatComp->FindNearestEnemy(CurActorLocation, MoveDistance+100, {}))
	{
		XMFLOAT3 EnemyLocation = NearestEnemy->GetActorLocation();
		WarpingTargetPos = GetWarpingPositionToEnemy(CurActorLocation, EnemyLocation, MoveDistance);
		
		MyGameCharacter->SetActorRotation(MyMath::GetRotationQuaternionToActor(CurActorLocation, EnemyLocation));
	}
	else
	{
		WarpingTargetPos = CurActorLocation + IfNoEnemyWarpingDirectionUnitVector * MoveDistance;
	}

	MotionWarpingComp->SetTargetLocation(WarpingTargetPos, MoveTime);
}

