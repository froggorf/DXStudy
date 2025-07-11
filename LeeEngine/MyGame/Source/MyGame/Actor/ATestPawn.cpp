﻿#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "ATestCube2.h"
#include "Engine/World/UWorld.h"


ATestPawn::ATestPawn()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
	}

	AssetManager::GetAsyncAssetCache("SK_MyUEFN",[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass("UMyAnimInstance");
	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SkeletalMeshComponent, "hand_r");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({-9.895, -3.056, -6.95});
	SMSword->SetRelativeRotation(XMFLOAT4{0.073,0.09,-0.638,0.761});

	TestComp = std::dynamic_pointer_cast<UTestComponent>(CreateDefaultSubobject("TestActorComp", "UTestComponent"));
}



void ATestPawn::BeginPlay()
{
	GEngine->GetWorld()->GetPlayerController()->OnPossess(this);

	ACharacter::BeginPlay();


	//CapsuleComp->SetDebugDraw(true);
	

	SMSword->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	SMSword->GetBodyInstance()->OnComponentHit.Add(this, &ATestPawn::OnComponentHitEvent);


	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SMSword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SMSword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);

	AssetManager::GetAsyncAssetCache("AM_Sword", [this](std::shared_ptr<UObject> Object)
		{
			AM_Sword = std::static_pointer_cast<UAnimMontage>(Object);
		});

	
}

void ATestPawn::BindKeyInputs()
{
	if (Controller)
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = Controller->PlayerInput)
		{
			InputSystem->BindAction(EKeys::MouseLeft, ETriggerEvent::Started, this, &ATestPawn::Attack);
			InputSystem->BindAxis2D(EKeys::W, ETriggerEvent::Trigger, 1, 0,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::S, ETriggerEvent::Trigger, -1, 0,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::D, ETriggerEvent::Trigger, 0, 1,this, &ATestPawn::Move);
			InputSystem->BindAxis2D(EKeys::A, ETriggerEvent::Trigger, 0, -1,this, &ATestPawn::Move);

			InputSystem->BindAction<ATestPawn>(EKeys::Space, ETriggerEvent::Started, this, &ATestPawn::Jump);

			InputSystem->BindAxis2D(EKeys::MouseXY2DAxis, ETriggerEvent::Trigger, 0,0, this, &ATestPawn::Look);

			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &ATestPawn::MouseRotateStart);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &ATestPawn::MouseRotateEnd);
		}
		
	}
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

void ATestPawn::Attack()
{
	if (SkeletalMeshComponent)
	{
		if (const std::shared_ptr<UAnimInstance>& AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			if (AM_Sword)
			{
				AnimInstance->Montage_Play(AM_Sword, 0.0f);
			}
		}
	}
}

void ATestPawn::Move(float X, float Y)
{
	XMFLOAT3 ForwardDirection = Controller->GetActorForwardVector();
	XMFLOAT3 RightDirection = Controller->GetActorRightVector();

	AddMovementInput(ForwardDirection, X);
	AddMovementInput(RightDirection, Y);
}

void ATestPawn::Look(float X, float Y)
{
	// 우클릭이 되어있다면
	if (bRightButtonPressed && Controller && Controller->PlayerInput)
	{
		XMFLOAT2 Delta = Controller->PlayerInput->LastMouseDelta;
		AddControllerYawInput(Delta.x);
		AddControllerPitchInput(Delta.y);
	}
}

void ATestPawn::MouseRotateStart()
{
	bRightButtonPressed = true;
}

void ATestPawn::MouseRotateEnd()
{
	bRightButtonPressed = false;
}


void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
