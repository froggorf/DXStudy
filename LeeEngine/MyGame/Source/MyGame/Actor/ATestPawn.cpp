#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "ATestCube2.h"
#include "Engine/World/UWorld.h"

void UCharacterMovementComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	GravityScale = 12.5f;
	MaxStepHeight = 20.0f;
	WalkableFloorAngle = 44.5f;
	MaxWalkSpeed = 500.0f;
	JumpZVelocity = 450.0f;
	Braking = 2048.0f;
	Acceleration = 2048.0f;

	Manager = PxCreateControllerManager(*gPhysicsEngine->GetScene());

	// UCharacterMovementComponent를 받는 것은 ACharacter 이며,
	// 무조건 RootComponent로 UCapsuleComponent를 가짐
	/*float GravityScale;
	float Mass;
	float MaxWalkSpeed;
	float JumpZVelocity;*/
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

ATestPawn::ATestPawn()
{
	Radius = 20.0f;
	HalfHeight = 65.0f;
	CapsuleComp = std::make_shared<UCapsuleComponent>();
	CapsuleComp->SetRadius(Radius);
	CapsuleComp->SetHalfHeight(HalfHeight);
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetWorldLocation({0,HalfHeight,0});
	


	SKComp = std::make_shared<USkeletalMeshComponent>();
	SKComp->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SK_MyUEFN",[this](std::shared_ptr<UObject> Object)
		{
			SKComp->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SKComp->SetAnimInstanceClass("UMyAnimInstance");
	SKComp->SetRelativeLocation({0,-85,0});

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SKComp, "hand_r");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({-9.895, -3.056, -6.95});
	SMSword->SetRelativeRotation(XMFLOAT4{0.073,0.09,-0.638,0.761});

	

	TestComp = std::dynamic_pointer_cast<UTestComponent>(CreateDefaultSubobject("TestActorComp", "UTestComponent"));

	
	MovementComp = std::dynamic_pointer_cast<UCharacterMovementComponent>( CreateDefaultSubobject("MovementComponent", "UCharacterMovementComponent"));
}



void ATestPawn::BeginPlay()
{
	AActor::BeginPlay();

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetObjectType(ECollisionChannel::Pawn);
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetKinematicRigidBody(true);
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		CapsuleComp->SetResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);	
	}
	
	CapsuleComp->SetDebugDraw(true);
	

	SMSword->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	SMSword->GetBodyInstance()->OnComponentHit.Add(this, &ATestPawn::OnComponentHitEvent);


	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SMSword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SMSword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);

	
}

void ATestPawn::OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults)
{
	MY_LOG("Hit",EDebugLogLevel::DLL_Warning, OtherActor->GetName() + " -> " + OtherComp->GetName());
}


void ATestPawn::AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	if (ATestCube2* TestCube = dynamic_cast<ATestCube2*>(OtherActor))
	{
		TestCube->TestCompFunc();
		
	}
}
void ATestPawn::AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{

	MY_LOG("End",EDebugLogLevel::DLL_Warning, OtherActor->GetName());
}

void ATestPawn::SetAttackStart()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(true);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	SMSword->GetBodyInstance()->SetKinematicRigidBody(true);
	SMSword->GetBodyInstance()->SetDebugDraw(true);
}

void ATestPawn::SetAttackEnd()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMSword->GetBodyInstance()->SetDebugDraw(false);
}

void ATestPawn::AddMovementInput(const XMFLOAT3& WorldDirection, float ScaleValue)
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = MovementComp.lock())
	{
		CharacterMovementComp->AddInputVector(WorldDirection,ScaleValue);
	}
}

void ATestPawn::Jump()
{
	if (std::shared_ptr<UCharacterMovementComponent> CharacterMovementComp = MovementComp.lock())
	{
		CharacterMovementComp->Jump();
	}
}


void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (ImGui::IsKeyDown(ImGuiKey_I)) AddMovementInput({0,0,1}, 1);
	if (ImGui::IsKeyDown(ImGuiKey_K)) AddMovementInput({0,0,-1}, 1);
	if (ImGui::IsKeyDown(ImGuiKey_J)) AddMovementInput({-1,0,0}, 1);
	if (ImGui::IsKeyDown(ImGuiKey_L)) AddMovementInput({1,0,0}, 1);

	if (ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		Jump();
	}
}
