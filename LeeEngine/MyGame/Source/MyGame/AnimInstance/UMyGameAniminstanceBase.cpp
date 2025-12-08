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

	XMFLOAT3 MoveVel = MovementComp->Velocity;
	MoveVel.y = 0.0f;
	MovementVelocity = XMVectorGetX(XMVector3Length(XMLoadFloat3(&MoveVel)));
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



// Note: 뭔가 더 좋은 방법이 있을것같은데 귀찮아서 단순히 나눔
bool UMyGameAnimInstanceBase::IsAllResourceOK()
{
	return UAnimInstance::IsAllResourceOK() && MovementComp && BS_Locomotion;
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
		if (AnimState != EAnimState::Locomotion)
		{
			int a= 0;
		}
		switch (AnimState)
		{
		case EAnimState::Locomotion:
			BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity }, CurrentTime, BoneTransforms, FinalNotifies);      	
		break;
		case EAnimState::Falling:
			{
			MY_LOG("LOG", EDebugLogLevel::DLL_Display, "Falling");
			float FallingTime = AS_Falling->GetDuration();
			if (CurrentFallingTime >= FallingTime)
			{
				CurrentFallingTime = FallingTime;
				LandingStart();
			}
			AS_Falling->GetBoneTransform(CurrentFallingTime, BoneTransforms, nullptr);
				CurrentFallingTime += dt;
			}
		break;
		case EAnimState::Jump:
			{
			MY_LOG("LOG", EDebugLogLevel::DLL_Display, "Jump");
			float JumpStartAnimTime = AS_JumpStart->GetDuration();
			if (CurrentJumpStartTime >= JumpStartAnimTime)
			{
				CurrentJumpStartTime= JumpStartAnimTime;
				FallingStart();
			}
			AS_JumpStart->GetBoneTransform(CurrentJumpStartTime, BoneTransforms, nullptr);
				CurrentJumpStartTime += dt*1500;
			}
		break;
		case EAnimState::Landing:
			{
				MY_LOG("LOG", EDebugLogLevel::DLL_Display, "Land");
				float LandingAnimDuration = AS_Land->GetDuration();
				if (CurrentLandingTime >= LandingAnimDuration)
				{
					CurrentLandingTime = LandingAnimDuration;
					AnimState = EAnimState::Locomotion;
				}
				static std::vector<FBoneLocalTransform> LocomotionBoneTransform(MAX_BONES,FBoneLocalTransform{});
				// Locomotion
				BS_Locomotion->GetAnimationBoneTransforms(XMFLOAT2{0.0f, MovementVelocity }, CurrentTime, LocomotionBoneTransform, FinalNotifies);      	
				// Landing 애니메이션 재생
				static std::vector<FBoneLocalTransform> LandBoneTransforms(MAX_BONES, FBoneLocalTransform{});
				AS_Land->GetBoneTransform(CurrentLandingTime, LandBoneTransforms, nullptr);

				for (int i = 0; i < MAX_BONES; ++i)
				{
					BoneTransforms[i] = Blend2BoneTransform(LocomotionBoneTransform[i], LandBoneTransforms[i], 1.0f - CurrentLandingTime / LandingAnimDuration);
				}
				 

				CurrentLandingTime += dt * 1500;

			}
		break;
		default:
			MY_LOG("LOG",EDebugLogLevel::DLL_Error, "!");
		}
		
	}

	// 몽타쥬 연결
	{
		PlayMontage("DefaultSlot", BoneTransforms, FinalNotifies);
	}

	//bool Dummy;
	//AS->GetBoneTransform(CurrentTime, BoneTransforms, &Dummy);
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

