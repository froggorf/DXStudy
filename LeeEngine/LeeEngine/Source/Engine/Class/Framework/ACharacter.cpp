#include "CoreMinimal.h"
#include "ACharacter.h"

#include "Engine/Class/Camera/UCameraComponent.h"
#include "Engine/Physics/UShapeComponent.h"
#include "Engine/World/UWorld.h"


physx::PxQueryHitType::Enum MyQueryFilterCallback::preFilter(const physx::PxFilterData&, const physx::PxShape*, const physx::PxRigidActor* actor, physx::PxHitFlags&)
{
	if (actor->userData)
	{
		if (IgnoreActor == static_cast<UShapeComponent*>(actor->userData)->GetOwner())
		{
			return physx::PxQueryHitType::eNONE; // 충돌 무시
		}	
	}

	return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum MyQueryFilterCallback::postFilter(
	const physx::PxFilterData&,
	const physx::PxQueryHit& hit,
	const physx::PxShape*,
	const physx::PxRigidActor* actor)
{
	if (!actor || !actor->userData)
	{
		return physx::PxQueryHitType::eBLOCK;
	}

	UShapeComponent* ShapeComp = static_cast<UShapeComponent*>(actor->userData);
	AActor* OtherOwner = ShapeComp ? ShapeComp->GetOwner() : nullptr;
	if (!OtherOwner)
	{
		return physx::PxQueryHitType::eBLOCK;
	}

	if (OtherOwner == IgnoreActor)
	{
		return physx::PxQueryHitType::eNONE;
	}

	if (dynamic_cast<ACharacter*>(OtherOwner))
	{
		const physx::PxLocationHit& LocationHit = static_cast<const physx::PxLocationHit&>(hit);
		if ((LocationHit.flags & physx::PxHitFlag::eNORMAL) && LocationHit.normal.y > 0.5f)
		{
			return physx::PxQueryHitType::eNONE;
		}
	}

	return physx::PxQueryHitType::eBLOCK;
}

UCharacterMovementComponent::UCharacterMovementComponent()
{
	GravityScale = 12.5f;
	MaxStepHeight = 30.0f;
	WalkableFloorAngle = 44.5f;
	MaxWalkSpeed = 500.0f;
	JumpZVelocity = 450.0f;
	Braking = 2048.0f;
	Acceleration = 2048.0f;
}

void UCharacterMovementComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	OwnerCharacter = dynamic_cast<ACharacter*>(GetOwner());
	physx::PxControllerManager* Manager = GPhysicsEngine->GetControllerManager();
	if (!Manager)
	{
#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
		exit(0);
#endif
	}

	desc.height = std::static_pointer_cast<UCapsuleComponent>(GetOwner()->GetRootComponent())->GetHalfHeight()*2;
	desc.radius = std::static_pointer_cast<UCapsuleComponent>(GetOwner()->GetRootComponent())->GetRadius();
	desc.position = physx::PxExtendedVec3(GetOwner()->GetActorLocation().x,GetOwner()->GetActorLocation().y,-GetOwner()->GetActorLocation().z);
	desc.material = GPhysicsEngine->GetDefaultMaterial();
	desc.stepOffset = MaxStepHeight;
	desc.slopeLimit = XMConvertToRadians(WalkableFloorAngle);

	// 컨트롤러 관찰자 생성 및 설정
	ControllerObserver = std::make_unique<FCharacterControllerObserver>(this);
	desc.reportCallback = ControllerObserver.get();

	PxCharacterController = Manager->createController(desc);

	CCTQueryCallBack.IgnoreActor = GetOwner();
	Filters.mFilterCallback = &CCTQueryCallBack;
	Filters.mFilterFlags = physx::PxQueryFlag::eSTATIC
		| physx::PxQueryFlag::eDYNAMIC
		| physx::PxQueryFlag::ePREFILTER
		| physx::PxQueryFlag::ePOSTFILTER;

	bIsFalling = false;
}

void UCharacterMovementComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);

	if (!PxCharacterController)
	{
		return;
	}

	/// 캐릭터 이동
	XMVECTOR InputDir = XMLoadFloat3(&ControlInputVector);
	float InputLen = XMVectorGetX(XMVector3Length(InputDir));
	XMVECTOR MoveDir = (InputLen > 0.01f) ? XMVector3Normalize(InputDir) : XMVectorZero();

	XMVECTOR CurVel = XMLoadFloat3(&Velocity);

	// 입력이 있으면 가속 진행
	if (InputLen > 0.01f)
	{
		XMVECTOR TargetVel = MoveDir * MaxWalkSpeed;
		XMVECTOR DeltaVel = TargetVel - CurVel;

		float DeltaVelLen = XMVectorGetX(XMVector3Length(DeltaVel));
		float MaxDelta = Acceleration * DeltaSeconds;

		if (DeltaVelLen > MaxDelta)
			DeltaVel = XMVector3Normalize(DeltaVel) * MaxDelta;

		CurVel += DeltaVel;
	}
	// 입력이 없을 때는 감속
	else
	{
		float Speed = XMVectorGetX(XMVector3Length(CurVel));
		float Drop = Braking * DeltaSeconds;

		if (Speed > Drop)
			CurVel -= XMVector3Normalize(CurVel) * Drop;
		else
			CurVel = XMVectorZero();
	}

	XMStoreFloat3(&Velocity, CurVel);

	CurVelocityY -= 9.8f*GPhysicsEngine->GetSceneDefaultGravityScale()*DeltaSeconds*GravityScale;
	physx::PxVec3 MoveVel = {Velocity.x,Velocity.y,-Velocity.z};
	MoveVel.y = CurVelocityY;
	physx::PxControllerCollisionFlags MoveFlags = PxCharacterController->move(MoveVel * DeltaSeconds, 0.01f, DeltaSeconds, Filters);

	if (MoveFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		CurVelocityY = 0;
	}

	physx::PxExtendedVec3 NewPos = PxCharacterController->getPosition();

	XMFLOAT3 NewP = {static_cast<float>(NewPos.x),static_cast<float>(NewPos.y),static_cast<float>(-NewPos.z)};
	XMFLOAT3 OldP = GetOwner()->GetActorLocation();

	physx::PxControllerState ControllerState;
	PxCharacterController->getState(ControllerState);

	bool bStandingOnCharacter = ControllerState.standOnAnotherCCT;
	if (!bStandingOnCharacter && ControllerState.touchedActor && ControllerState.touchedActor->userData)
	{
		UShapeComponent* ShapeComp = static_cast<UShapeComponent*>(ControllerState.touchedActor->userData);
		AActor* OtherOwner = ShapeComp ? ShapeComp->GetOwner() : nullptr;
		if (dynamic_cast<ACharacter*>(OtherOwner))
		{
			bStandingOnCharacter = true;
		}
	}

	if (bStandingOnCharacter && CurVelocityY <= 0.0f && NewP.y > OldP.y)
	{
		NewP.y = OldP.y;
	}

	GetOwner()->SetActorLocation(NewP);

	ControlInputVector = {0,0,0};
	/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// 캐릭터 회전
	// bOrientRotationToMovement == false;
	if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&Velocity))) > FLT_EPSILON && !bOrientRotationToMovement && GetOwner())
	{
		XMFLOAT4 CharacterRotation = GetOwner()->GetActorRotation();
		XMVECTOR CharacterRotationQuat = XMLoadFloat4(&CharacterRotation);

		// Forward Vector 를 Velocity 방향벡터 점이 될 수 있도록 회전 쿼터니언을 구한다음에
		XMVECTOR WorldForward = XMVectorSet(0, 0, 1, 0);
		XMFLOAT3 ToRotation = XMFLOAT3{Velocity.x,0, Velocity.z};
		XMVECTOR ToVector = XMVector3Normalize(XMLoadFloat3(&ToRotation));
		XMVECTOR Axis = XMVector3Cross(WorldForward, ToVector);
		float Dot = XMVectorGetX(XMVector3Dot(WorldForward, ToVector));
		float Angle = acosf(Dot);
		XMVECTOR MoveVelocityQuat;

		if (XMVector3Equal(Axis, XMVectorZero()))
		{
			if (Dot > 0)
			{
				MoveVelocityQuat = XMQuaternionIdentity();
			}
			else
			{
				MoveVelocityQuat = XMQuaternionRotationAxis(XMVectorSet(0,1,0,0), XM_PI);
			}
				
		}
		else
		{
			MoveVelocityQuat = XMQuaternionRotationAxis(XMVector3Normalize(Axis), Angle);
		}
		// 그 쿼터니언과 현재 캐릭터 RotationQuat을 보간하고
		float RotationSpeed = RotationRate.y; 
		float MaxStep = RotationSpeed * DeltaSeconds; 
		float AngleBetween;
		XMQuaternionToAxisAngle(&Axis, &AngleBetween, MoveVelocityQuat * XMQuaternionInverse(CharacterRotationQuat));
		AngleBetween = XMConvertToDegrees(AngleBetween);

		float T = 1.0f;
		if (AngleBetween > MaxStep && AngleBetween > 0.001f)
		{
			T = MaxStep / AngleBetween;
			T = std::clamp(T, 0.0f, 1.0f);
		}

		XMVECTOR NewQuat = XMQuaternionSlerp(CharacterRotationQuat, MoveVelocityQuat, T);

		XMFLOAT4 NewRot;
		XMStoreFloat4(&NewRot,NewQuat);
		
		// 그 값을 적용하면 됨
		GetOwner()->SetActorRotation(NewRot);
	}
	if (bOrientRotationToMovement)
	{
		XMFLOAT4 ControlRotation = static_cast<ACharacter*>(GetOwner())->GetControlRotation();
		XMFLOAT3 RotationEulerRadian = MyMath::QuaternionToEulerAngle(ControlRotation);

		RotationEulerRadian.x = bUseControllerRotationPitch? RotationEulerRadian.x : 0.0f;
		RotationEulerRadian.y = bUseControllerRotationYaw? RotationEulerRadian.y : 0.0f;
		RotationEulerRadian.z = bUseControllerRotationRoll? RotationEulerRadian.z : 0.0f;
		XMVECTOR NewRotationQuat = XMQuaternionRotationRollPitchYaw(RotationEulerRadian.x,RotationEulerRadian.y,RotationEulerRadian.z);
		GetOwner()->SetActorRotation(NewRotationQuat);
	}
	/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void UCharacterMovementComponent::AddInputVector(XMFLOAT3 WorldAccel, float Power)
{
	ControlInputVector.x += WorldAccel.x * Power;
	ControlInputVector.y += WorldAccel.y * Power;
	ControlInputVector.z += WorldAccel.z * Power;
}

void UCharacterMovementComponent::Jump()
{
	CurVelocityY = JumpZVelocity;
	Velocity.y = JumpZVelocity;
	bIsFalling = true;
}

void UCharacterMovementComponent::Landing()
{
	bIsFalling = false;

	if (OwnerCharacter)
	{
		OwnerCharacter->Landing();
	}
}

ACharacter::ACharacter()
{
	Radius = 20.0f;
	HalfHeight = 65.0f;
	CapsuleComp = std::make_shared<UCapsuleComponent>();
	CapsuleComp->SetRadius(Radius);
	CapsuleComp->SetHalfHeight(HalfHeight);
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetWorldLocation({0,HalfHeight,0});

	QueryCheckCapsuleComp = std::make_shared<UStaticMeshComponent>();
	
	QueryCheckCapsuleComp->SetupAttachment(GetRootComponent());
	


	SkeletalMeshComponent = std::make_shared<USkeletalMeshComponent>();
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	CharacterMovement = std::dynamic_pointer_cast<UCharacterMovementComponent>( CreateDefaultSubobject("MovementComponent", "UCharacterMovementComponent"));

	SpringArm = std::make_shared<USpringArmComponent>();
	SpringArm->SetArmLength(300.0f);
	SpringArm->SetupAttachment(CapsuleComp);

	CameraComp = std::make_shared<UCameraComponent>();
	CameraComp->SetupAttachment(SpringArm, "");
	CameraComp->Rename("PlayerCameraComp");
}

void ACharacter::Register()
{
	// 디버깅 테스트용
	AActor::Register();

	QueryCheckCapsuleComp->SetStaticMesh((CapsuleComp->MakeStaticMesh()));
	QueryCheckCapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	QueryCheckCapsuleComp->GetBodyInstance()->SetSimulatePhysics(false);
}

void ACharacter::BeginPlay()
{
	AActor::BeginPlay();

	QueryCheckCapsuleComp->SetVisibility(false);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComp->SetCollisionObjectType(ECollisionChannel::Pawn);
	CapsuleComp->SetObjectType(ECollisionChannel::Pawn);
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetKinematicRigidBody(true);

}

void ACharacter::PossessedBy(AController* NewController)
{
	Controller = NewController;
}

void ACharacter::UnPossessed()
{
	Controller = nullptr;
}

void ACharacter::SetActorLocation(const XMFLOAT3& NewLocation) const
{
	AActor::SetActorLocation(NewLocation);

	SyncCapsulePhysicsActors(NewLocation);
}

void ACharacter::SyncCapsulePhysicsActors(const XMFLOAT3& NewLocation) const
{
	XMFLOAT4 Rotation = GetActorRotation();

	physx::PxTransform NewTransform(
		physx::PxVec3(NewLocation.x, NewLocation.y, -NewLocation.z),
		physx::PxQuat(-Rotation.x, -Rotation.y, Rotation.z, Rotation.w)
	);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		if (Movement->PxCharacterController)
		{
			Movement->PxCharacterController->setPosition(physx::PxExtendedVec3{NewTransform.p.x,NewTransform.p.y,NewTransform.p.z});	
		}
		
	}

	// ✅ CapsuleComp 동기화
	if (CapsuleComp && CapsuleComp->GetRigidActor())
	{
		CapsuleComp->GetRigidActor()->setGlobalPose(NewTransform);
	}

	// ✅ QueryCheckCapsuleComp 동기화
	if (QueryCheckCapsuleComp && QueryCheckCapsuleComp->GetBodyInstance()&& QueryCheckCapsuleComp->GetBodyInstance()->GetRigidActor())
	{
		QueryCheckCapsuleComp->GetBodyInstance()->GetRigidActor()->setGlobalPose(NewTransform);
	}
}

void ACharacter::SetActorLocation_Teleport(const XMFLOAT3& NewLocation) const
{
	
	SetActorLocation(NewLocation);
}


void ACharacter::AddMovementInput(const XMFLOAT3& WorldDirection, float ScaleValue)
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = CharacterMovement.lock())
	{
		if (Controller && !Controller->IsPlayRootMotion())
		{
			CharacterMovementComp->AddInputVector(WorldDirection,ScaleValue);	
		}
	}
}

void ACharacter::AddControllerYawInput(float Val)
{
	Val /= 10;
	APlayerController* PC = dynamic_cast<APlayerController*>(Controller);
	if (PC && std::abs(Val) > FLT_EPSILON)
	{
		PC->AddYawInput(Val);
	}
}

void ACharacter::AddControllerPitchInput(float Val)
{
	Val /= 10;
	APlayerController* PC = dynamic_cast<APlayerController*>(Controller);
	if (PC && std::abs(Val) > FLT_EPSILON)
	{
		PC->AddPitchInput(Val);
	}
}

void ACharacter::Jump()
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = CharacterMovement.lock())
	{
		CharacterMovementComp->Jump();
	}

	if (const std::shared_ptr<UAnimInstance>& AnimInstance = GetAnimInstance())
	{
		AnimInstance->JumpStart();
	}
}

void ACharacter::Landing()
{
	if (const std::shared_ptr<UAnimInstance>& AnimInstance = GetAnimInstance())
	{
		AnimInstance->LandingStart();
	}
}

void ACharacter::SetControlRotation(const XMFLOAT4& NewRot)
{
	ControlRotation = NewRot;
}

std::shared_ptr<UAnimInstance> ACharacter::GetAnimInstance() const
{ 
	if (USkeletalMeshComponent* Mesh = GetSkeletalMeshComponent())
	{
		if (const std::shared_ptr<UAnimInstance>& AnimInstance = Mesh->GetAnimInstance())
		{
			return AnimInstance;
		}
	}
	return nullptr;
}

void ACharacter::HandleRootMotion(const XMMATRIX& Root)
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (!MovementComp)
		return;

	if (LastPlayingAnimMontage != CurPlayingAnimMontage || LastUpdateTime + 0.25f < GEngine->GetTimeSeconds())
	{
		LastPlayingAnimMontage = CurPlayingAnimMontage;
		PreviousRootMatrix = Root;
		LastUpdateTime = GEngine->GetTimeSeconds();
	}

	XMMATRIX DeltaMatrix = XMMatrixMultiply(XMMatrixInverse(nullptr, PreviousRootMatrix), Root);

	XMFLOAT3 RawDelta = {
		DeltaMatrix.r[3].m128_f32[0], 
		DeltaMatrix.r[3].m128_f32[1], 
		DeltaMatrix.r[3].m128_f32[2]
	};
	XMVECTOR DeltaPos = XMVectorSet(RawDelta.x, RawDelta.y, -RawDelta.z, 0.0f);

	XMFLOAT3 ConvertedDelta;
	XMStoreFloat3(&ConvertedDelta, DeltaPos);

	XMFLOAT4 ActorRotation = GetActorRotation();
	XMVECTOR ActorQuat = XMLoadFloat4(&ActorRotation);
	XMVECTOR WorldDelta = XMVector3Rotate(DeltaPos, ActorQuat);

	XMFLOAT3 WorldDeltaPos;
	XMStoreFloat3(&WorldDeltaPos, WorldDelta);
	WorldDeltaPos.y = 0;

	// 이동 적용
	if (MovementComp->PxCharacterController)
	{
		physx::PxVec3 PxDelta(WorldDeltaPos.x, WorldDeltaPos.y, -WorldDeltaPos.z);
		MovementComp->PxCharacterController->move(
			PxDelta, 
			0.01f, 
			GEngine->GetDeltaSeconds(), 
			MovementComp->Filters
		);
	}

	PreviousRootMatrix = Root;
	LastUpdateTime = GEngine->GetTimeSeconds();
}

