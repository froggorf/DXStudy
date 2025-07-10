#include "CoreMinimal.h"
#include "ACharacter.h"

#include "Engine/Class/Camera/UCameraComponent.h"
#include "Engine/World/UWorld.h"


UCharacterMovementComponent::UCharacterMovementComponent()
{
	GravityScale = 12.5f;
	MaxStepHeight = 20.0f;
	WalkableFloorAngle = 44.5f;
	MaxWalkSpeed = 500.0f;
	JumpZVelocity = 450.0f;
	Braking = 2048.0f;
	Acceleration = 2048.0f;
}

void UCharacterMovementComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	Manager = PxCreateControllerManager(*gPhysicsEngine->GetScene());

	desc.height = std::static_pointer_cast<UCapsuleComponent>(GetOwner()->GetRootComponent())->GetHalfHeight()*2;
	desc.radius = std::static_pointer_cast<UCapsuleComponent>(GetOwner()->GetRootComponent())->GetRadius();
	desc.position = physx::PxExtendedVec3(GetOwner()->GetActorLocation().x,GetOwner()->GetActorLocation().y,-GetOwner()->GetActorLocation().z);
	desc.material = gPhysicsEngine->GetDefaultMaterial();
	desc.stepOffset = MaxStepHeight;
	desc.slopeLimit = XMConvertToRadians(WalkableFloorAngle);

	Controller = Manager->createController(desc);

	CCTQueryCallBack.IgnoreActor = GetOwner();
	Filters.mFilterCallback = &CCTQueryCallBack;
}

void UCharacterMovementComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);

	if (!Controller)
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

	CurVelocityY -= 9.8f*gPhysicsEngine->GetSceneDefaultGravityScale()*DeltaSeconds*GravityScale;
	physx::PxVec3 MoveVel = {Velocity.x,Velocity.y,-Velocity.z};
	MoveVel.y = CurVelocityY;
	physx::PxControllerCollisionFlags MoveFlags = Controller->move(MoveVel * DeltaSeconds, 0.01f, DeltaSeconds, Filters);

	if (MoveFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		CurVelocityY = 0;
	}

	physx::PxExtendedVec3 NewPos = Controller->getPosition();

	XMFLOAT3 NewP = {static_cast<float>(NewPos.x),static_cast<float>(NewPos.y),static_cast<float>(-NewPos.z)};
	GetOwner()->SetActorLocation(NewP);

	ControlInputVector = {0,0,0};
	/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// 캐릭터 회전
	if (XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&Velocity))) > FLT_EPSILON && bOrientRotationToMovement && GetOwner())
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

	SkeletalMeshComponent = std::make_shared<USkeletalMeshComponent>();
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	CharacterMovement = std::dynamic_pointer_cast<UCharacterMovementComponent>( CreateDefaultSubobject("MovementComponent", "UCharacterMovementComponent"));

	SpringArm = std::make_shared<USpringArmComponent>();
	SpringArm->SetArmLength(300.0f);
	SpringArm->SetupAttachment(CapsuleComp);

	CameraComp = std::make_shared<UCameraComponent>();
	CameraComp->SetupAttachment(SpringArm, "");
	
}

void ACharacter::BeginPlay()
{
	AActor::BeginPlay();

	BindKeyInputs();

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetObjectType(ECollisionChannel::Pawn);
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetKinematicRigidBody(true);
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		CapsuleComp->SetResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);	
	}

	GEngine->GetWorld()->GetCameraManager()->SetTargetCamera(CameraComp);
}

void ACharacter::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void ACharacter::AddMovementInput(const XMFLOAT3& WorldDirection, float ScaleValue)
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = CharacterMovement.lock())
	{
		CharacterMovementComp->AddInputVector(WorldDirection,ScaleValue);
	}
}

void ACharacter::AddControllerYawInput(float Val)
{
	if (Controller && std::abs(Val) > FLT_EPSILON)
	{
		Controller->AddYawInput(Val);
	}
}

void ACharacter::AddControllerPitchInput(float Val)
{
	if (Controller && std::abs(Val) > FLT_EPSILON )
	{
		Controller->AddPitchInput(Val);
	}
}

void ACharacter::Jump()
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = CharacterMovement.lock())
	{
		CharacterMovementComp->Jump();
	}
}

void ACharacter::SetControlRotation(const XMFLOAT4& NewRot)
{
	ControlRotation = NewRot;

}

