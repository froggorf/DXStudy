#include "CoreMinimal.h"
#include "UMyGameAniminstanceBase.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
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

	AssetManager::GetAsyncAssetCache(GetJumpStartAnimSequenceName(), [this](std::shared_ptr<UObject> Object)
		{
			AS_JumpStart = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});

	AssetManager::GetAsyncAssetCache(GetFallingAnimSequenceName(), [this](std::shared_ptr<UObject> Object)
		{
			AS_Falling = std::dynamic_pointer_cast<UAnimSequence>(Object);
		});

	AssetManager::GetAsyncAssetCache(GetLandAnimSequenceName(), [this](std::shared_ptr<UObject> Object)
		{
			AS_Land = std::dynamic_pointer_cast<UAnimSequence>(Object);
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
		if (ACharacter* Character = dynamic_cast<ACharacter*>(OwnerActor))
		{
			MovementComp = Character->GetCharacterMovement();
		}
	}
}

void UMyGameAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComp)
	{
		XMFLOAT3 MoveVel = MovementComp->Velocity;
		MoveVel.y = 0.0f;
		MovementVelocity = XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel)));	
	}
	
}

XMFLOAT3 UMyGameAnimInstanceBase::GetWarpingPositionToEnemy(const XMFLOAT3& CurActorLocation, XMFLOAT3& EnemyLocation, float MaxMoveDistance)
{
	// y좌표 이동을 제어하기위해
	EnemyLocation.y = CurActorLocation.y;

	float ToEnemyDistance = MyMath::GetDistance(CurActorLocation, EnemyLocation);
	XMFLOAT3 ToEnemyVector = MyMath::GetDirectionUnitVector(CurActorLocation, EnemyLocation);

	// 적군의 길이 30 -> 적의 위치까지 가려다가 충돌체크가 이상하게 되는 경우를 방지하기 위해 약간의 갭을 두고 이동
	// std::min(max(ToEnemyDistance - 30, 0), MaxMoveDistance) -> 이동할 수 있는 거리와 적군 앞까지의 거리중 작은것을 선택
	return CurActorLocation + ToEnemyVector * std::min(max(ToEnemyDistance - 150, 0), MaxMoveDistance);
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
	return UAnimInstance::IsAllResourceOK() && MovementComp && BS_Locomotion;
}

void UMyGameAnimInstanceBase::CalculateAnimByState(EAnimState State, float AnimTime, std::vector<FBoneLocalTransform>& OutBoneTransform)
{
	switch (AnimState)
	{
	case EAnimState::Locomotion:
		BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity }, CurrentTime, OutBoneTransform, FinalNotifies);      	
		break;
	case EAnimState::Falling:
	{
		AS_Falling->GetBoneTransform(CurrentFallingTime, OutBoneTransform, nullptr);
		
	}
	break;
	case EAnimState::Jump:
	{
		AS_JumpStart->GetBoneTransform(CurrentJumpStartTime, OutBoneTransform, nullptr);
	}
	break;
	case EAnimState::Landing:
	{
		AS_Land->GetBoneTransform(CurrentLandingTime, OutBoneTransform, nullptr);
	}
	break;
	default:
		MY_LOG("LOG",EDebugLogLevel::DLL_Error, "!");
	}
}

void UMyGameAnimInstanceBase::UpdateAnimationState(float dt, std::vector<FBoneLocalTransform>& OutBoneTransform)
{
	static float AnimTimeMultiply = 1500.f;
	switch (AnimState)
	{
	case EAnimState::Locomotion:
		BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity}, CurrentTime, OutBoneTransform, FinalNotifies);
		break;

	case EAnimState::Jump:
	{
		float JumpStartAnimTime = AS_JumpStart->GetDuration();
		AS_JumpStart->GetBoneTransform(CurrentJumpStartTime, OutBoneTransform, nullptr);

		// 점프 시간 업데이트 및 상태 전환 체크
		CurrentJumpStartTime += dt * AnimTimeMultiply * GetJumpStartTimeMultiply();
		if (CurrentJumpStartTime >= JumpStartAnimTime)
		{
			CurrentJumpStartTime = JumpStartAnimTime;
			FallingStart();
		}
	}
	break;

	case EAnimState::Falling:
	{
		float FallingTime = AS_Falling->GetDuration();
		AS_Falling->GetBoneTransform(CurrentFallingTime, OutBoneTransform, nullptr);

		// 낙하 시간 업데이트 및 상태 전환 체크
		CurrentFallingTime += dt;
		if (CurrentFallingTime >= FallingTime)
		{
			CurrentFallingTime = FallingTime;
			LandingStart();
		}
	}
	break;

	case EAnimState::Landing:
	{
		std::vector<FBoneLocalTransform> LandingAnim(MAX_BONES);
		std::vector<FBoneLocalTransform> LocomotionAnim(MAX_BONES);

		float LandingAnimDuration = AS_Land->GetDuration();
			AS_Land->GetBoneTransform(CurrentLandingTime, LandingAnim, nullptr);
		BS_Locomotion->GetAnimationBoneTransforms(
			XMFLOAT2{0.0f, MovementVelocity}, 
			CurrentTime, 
			LocomotionAnim, 
			FinalNotifies
		);

		float BlendAlpha = std::clamp(CurrentLandingTime / LandingAnimDuration, 0.0f, 1.0f);

		for (int i = 0; i < MAX_BONES; ++i)
		{
			OutBoneTransform[i] = Blend2BoneTransform(
				LandingAnim[i],      
				LocomotionAnim[i],   
				BlendAlpha           
			);
		}

		CurrentLandingTime += dt * AnimTimeMultiply * GetLandingTimeMultiply();

		if (CurrentLandingTime >= LandingAnimDuration)
		{
			CurrentLandingTime = 0.0f;  
			ChangeAnimState(EAnimState::Locomotion);
		}

		break;
	}
	break;

	default:
		MY_LOG("LOG", EDebugLogLevel::DLL_Error, "Unknown animation state!");
		break;
	}
}

void UMyGameAnimInstanceBase::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);

	if (!IsAllResourceOK())
	{
		return;
	}

	// 현재 상태와 이전 상태의 애니메이션 계산
	std::vector<FBoneLocalTransform> CurrentStateBones(MAX_BONES, FBoneLocalTransform{});
	std::vector<FBoneLocalTransform> PreviousStateBones(MAX_BONES, FBoneLocalTransform{});

	// 현재 상태의 애니메이션 계산
	UpdateAnimationState(dt, CurrentStateBones);

	// 상태 전환 중이라면 이전 상태의 애니메이션도 계산
	if (AnimationStateTransitionTime < 0.5f && LastAnimState != AnimState)
	{
		CalculateAnimByState(LastAnimState, CurrentTime, PreviousStateBones);

		// 블렌드 가중치 계산 (0.5초에 걸쳐 0->1로 변화)
		float BlendWeight = AnimationStateTransitionTime / 0.5f;

		// 두 애니메이션 블렌딩
		for (int i = 0; i < MAX_BONES; ++i)
		{
			BoneTransforms[i] = Blend2BoneTransform(PreviousStateBones[i], CurrentStateBones[i], BlendWeight);
		}

		// 전환 시간 업데이트
		AnimationStateTransitionTime += dt;
	}
	else
	{
		// 전환이 끝났거나 전환 중이 아니면 현재 상태 애니메이션만 사용
		BoneTransforms = CurrentStateBones;
	}

	// 몽타쥬 연결
	{
		PlayMontage("DefaultSlot", BoneTransforms, FinalNotifies);
	}
}

void UMyGameAnimInstanceBase::SetWarpingTarget(const XMFLOAT3& IfNoEnemyWarpingDirectionUnitVector, float MoveDistance, float MoveTime)
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

