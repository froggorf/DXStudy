#include "CoreMinimal.h"
#include "AMyGameCharacterBase.h"

#include "Engine/World/UWorld.h"


AMyGameCharacterBase::AMyGameCharacterBase()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 600.0f;
		CharacterMovement->Braking = 4096;
	}

	SpringArm->SetArmLength(250);
	//SpringArm->SetRelativeLocation({0,50,-50.0f});

	AssetManager::GetAsyncAssetCache("SK_Manny_UE4",[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass("UMyGameAnimInstanceBase");
	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});

	// AnimMontage
	{
		// Dodge
		{
			AssetManager::GetAsyncAssetCache("AM_UE4_Dodge_Fwd", [this](std::shared_ptr<UObject> Object)
				{
					AM_Dodge[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Dodge_Bwd", [this](std::shared_ptr<UObject> Object)
				{
					AM_Dodge[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Dodge_Left", [this](std::shared_ptr<UObject> Object)
				{
					AM_Dodge[static_cast<int>(EDodgeDirection::Left)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Dodge_Right", [this](std::shared_ptr<UObject> Object)
				{
					AM_Dodge[static_cast<int>(EDodgeDirection::Right)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});		
		}

		// Roll
		{
			AssetManager::GetAsyncAssetCache("AM_UE4_Roll_Fwd", [this](std::shared_ptr<UObject> Object)
				{
					AM_Roll[static_cast<int>(EDodgeDirection::Forward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Roll_Bwd", [this](std::shared_ptr<UObject> Object)
				{
					AM_Roll[static_cast<int>(EDodgeDirection::Backward)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Roll_Left", [this](std::shared_ptr<UObject> Object)
				{
					AM_Roll[static_cast<int>(EDodgeDirection::Left)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
			AssetManager::GetAsyncAssetCache("AM_UE4_Roll_Right", [this](std::shared_ptr<UObject> Object)
				{
					AM_Roll[static_cast<int>(EDodgeDirection::Right)] = std::dynamic_pointer_cast<UAnimMontage>(Object);
				});
		}
	}
}



void AMyGameCharacterBase::BeginPlay()
{
	GEngine->GetWorld()->GetPlayerController()->OnPossess(this);

	ACharacter::BeginPlay();
}

void AMyGameCharacterBase::BindKeyInputs()
{
	if (Controller)
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = Controller->GetPlayerInput())
		{
			// Walk / Run Toggle
			//InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &AMyGameCharacterBase::SetWalk);
			//InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Released, this, &AMyGameCharacterBase::SetRun);
			InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &AMyGameCharacterBase::Dodge);


			// Locomotion
			InputSystem->BindAxis2D(EKeys::W, ETriggerEvent::Trigger, 1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::S, ETriggerEvent::Trigger, -1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::D, ETriggerEvent::Trigger, 0, 1,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::A, ETriggerEvent::Trigger, 0, -1,this, &AMyGameCharacterBase::Move);

			// Jump
			InputSystem->BindAction<AMyGameCharacterBase>(EKeys::Space, ETriggerEvent::Started, this, &AMyGameCharacterBase::Jump);
			// LookRotate
			InputSystem->BindAxis2D(EKeys::MouseXY2DAxis, ETriggerEvent::Trigger, 0,0, this, &AMyGameCharacterBase::Look);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &AMyGameCharacterBase::MouseRotateStart);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &AMyGameCharacterBase::MouseRotateEnd);

			// TargetArmLength Wheel
			InputSystem->BindAction(EKeys::MouseWheelUp, Started, this, &AMyGameCharacterBase::WheelUp);
			InputSystem->BindAction(EKeys::MouseWheelDown, Started, this, &AMyGameCharacterBase::WheelDown);
		}
		
	}
}

//void ATestPawn::Backstep()
//{
//	if (SkeletalMeshComponent)
//	{
//		if (const std::shared_ptr<UAnimInstance>& AnimInstance = SkeletalMeshComponent->GetAnimInstance())
//		{
//			if (AM_Smash)
//			{
//				AnimInstance->Montage_Play(AM_Smash, 0.0f);
//			}
//		}
//	}
//}

void AMyGameCharacterBase::Move(float X, float Y)
{
	XMFLOAT3 ForwardDirection = Controller->GetActorForwardVector();
	XMFLOAT3 RightDirection = Controller->GetActorRightVector();

	AddMovementInput(ForwardDirection, X);
	AddMovementInput(RightDirection, Y);
}

void AMyGameCharacterBase::Look(float X, float Y)
{
	// 우클릭이 되어있다면
	if (bRightButtonPressed && Controller && Controller->GetPlayerInput())
	{
		XMFLOAT2 Delta = Controller->GetPlayerInput()->LastMouseDelta;
		AddControllerYawInput(Delta.x);
		AddControllerPitchInput(Delta.y);
	}
}

void AMyGameCharacterBase::MouseRotateStart()
{
	bRightButtonPressed = true;
}

void AMyGameCharacterBase::MouseRotateEnd()
{
	bRightButtonPressed = false;
}

void AMyGameCharacterBase::Dodge()
{
	
}

void AMyGameCharacterBase::SetWalk()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 150.0f;
	}
}

void AMyGameCharacterBase::SetRun()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 600.0f;
	}
}


void AMyGameCharacterBase::WheelUp()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength - 10.0f);
	}
}

void AMyGameCharacterBase::WheelDown()
{
	if (SpringArm)
	{
		SpringArm->SetArmLength(SpringArm->TargetArmLength + 10.0f);
	}
}


void AMyGameCharacterBase::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	
}
