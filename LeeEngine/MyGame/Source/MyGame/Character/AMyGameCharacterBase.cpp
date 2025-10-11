#include "CoreMinimal.h"
#include "AMyGameCharacterBase.h"

#include "Engine/World/UWorld.h"




AMyGameCharacterBase::AMyGameCharacterBase()
{
	if (!GDirectXDevice) return;

	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 600;
		CharacterMovement->Braking = 4096;
	}

	SpringArm->SetArmLength(250);
	

	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});


	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetIsMonochromeObject(false);
	}

	MotionWarpingComponent = std::static_pointer_cast<UMotionWarpingComponent>(CreateDefaultSubobject("MotionWarpingComp", "UMotionWarpingComponent"));

	UltimateSceneCameraComp = std::make_shared<UCameraComponent>();
	//UltimateSceneCameraComp->SetupAttachment(UltimateSceneSpringArm, "");



	
}

void AMyGameCharacterBase::Register()
{
	LoadCharacterData_OnRegister();

	ACharacter::Register();

	IDodgeInterface::LoadAnimMontages();

	if (CombatComponent)
	{
		CombatComponent->Initialize(this);
	}
	if (SkillComponent)
	{
		SkillComponent->Initialize(this);
	}
	if (UltimateComponent)
	{
		UltimateComponent->Initialize(this);
	}
}

void AMyGameCharacterBase::LoadCharacterData_OnRegister()
{
	AssetManager::GetAsyncAssetCache(CharacterMeshName,[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceName);

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	static constexpr bool bFindCameraRotation = false;
	if (bFindCameraRotation)
	{
		TestComp_DeleteLater = std::make_shared<USceneComponent>();

		SM_Arrow =  std::make_shared<UStaticMeshComponent>();
		SM_Arrow->SetupAttachment(TestComp_DeleteLater);
		SM_Arrow->SetRelativeRotation(XMFLOAT3{0,-90,0});
		AssetManager::GetAsyncAssetCache("SM_Arrow", [this](std::shared_ptr<UObject> Object)
			{
				SM_Arrow->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
			});
		SM_Arrow->Rename("Arrow");
		SM_Arrow->SetRelativeScale3D({10,10,10});
	}
#endif
}


void AMyGameCharacterBase::BeginPlay()
{
	GetWorld()->GetPlayerController()->OnPossess(this);

	ACharacter::BeginPlay();

	// Player 콜리젼 채널로 변경
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Player);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Player);

	CreateWidgetOnBeginPlay();
}

void AMyGameCharacterBase::BindKeyInputs()
{
	if (Controller)
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = Controller->GetPlayerInput())
		{
			// Locomotion
			InputSystem->BindAxis2D(EKeys::W, ETriggerEvent::Trigger, 1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::S, ETriggerEvent::Trigger, -1, 0,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::D, ETriggerEvent::Trigger, 0, 1,this, &AMyGameCharacterBase::Move);
			InputSystem->BindAxis2D(EKeys::A, ETriggerEvent::Trigger, 0, -1,this, &AMyGameCharacterBase::Move);
			// Dodge
			InputSystem->BindAction(EKeys::LShift, ETriggerEvent::Started, this, &AMyGameCharacterBase::Dodge);
			// Jump
			InputSystem->BindAction<AMyGameCharacterBase>(EKeys::Space, ETriggerEvent::Started, this, &AMyGameCharacterBase::Jump);
			// LookRotate
			InputSystem->BindAxis2D(EKeys::MouseXY2DAxis, ETriggerEvent::Trigger, 0,0, this, &AMyGameCharacterBase::Look);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &AMyGameCharacterBase::MouseRotateStart);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &AMyGameCharacterBase::MouseRotateEnd);

			// TargetArmLength Wheel
			InputSystem->BindAction(EKeys::MouseWheelUp, Started, this, &AMyGameCharacterBase::WheelUp);
			InputSystem->BindAction(EKeys::MouseWheelDown, Started, this, &AMyGameCharacterBase::WheelDown);

			if (CombatComponent)
			{
				CombatComponent->BindKeyInputs(InputSystem);
			}
			if (SkillComponent)
			{
				SkillComponent->BindKeyInputs(InputSystem);
			}
			if (UltimateComponent)
			{
				UltimateComponent->BindKeyInputs(InputSystem);
			}
		}
		
	}
}

void AMyGameCharacterBase::AttackedWhileDodge()
{
	SetTickRate(0.1f);
	GEngine->GetTimerManager()->SetTimer(RollingEndHandle, {this, &AMyGameCharacterBase::RollEnd} , 1.0f, false);
	AddMonochromePostprocess();
}

void AMyGameCharacterBase::ChangeToUltimateCamera()
{
	GEngine->GetWorld()->GetCameraManager()->SetTargetCamera(UltimateSceneCameraComp);
}

void AMyGameCharacterBase::ChangeToNormalCamera(float BlendTime)
{
	GEngine->GetWorld()->GetCameraManager()->SetViewTargetWithBlend(CameraComp, BlendTime, EViewTargetBlendFunction::Cubic);
}


float AMyGameCharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	if (bIsDodging)
	{
		ChangeToRoll();
		return DamageAmount;
	}
	

	return DamageAmount;
}

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
	std::shared_ptr<UAnimInstance> AnimInstance = GetAnimInstance();
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (!AnimInstance || !MovementComp)
	{
		return;
	}

	bIsDodging = true;
	Delegate<> OnDodgeEnd;
	OnDodgeEnd.Add(this, &AMyGameCharacterBase::DodgeEnd);

	// TODO: 수정하기
	if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_D))
	{
		bIsBackDodge = false;

		AnimInstance->Montage_Play(AM_Dodge[static_cast<int>(EDodgeDirection::Forward)], 0, OnDodgeEnd);
	}
	else
	{
		bIsBackDodge = true;
		AnimInstance->Montage_Play(AM_Dodge[static_cast<int>(EDodgeDirection::Backward)],0, OnDodgeEnd);
	}
}

void AMyGameCharacterBase::DodgeEnd()
{
	IDodgeInterface::DodgeEnd();
}

void AMyGameCharacterBase::ChangeToRoll()
{
	std::shared_ptr<UAnimInstance> AnimInstance = GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	
	bIsDodging = false;
	GEngine->GetTimerManager()->SetTimer(AttackedWhileDodgingHandle, {this, &AMyGameCharacterBase::AttackedWhileDodge} , AttackedWhileDodgeTriggerTime, false);

	const std::shared_ptr<UAnimMontage>& PlayedMontage = bIsBackDodge? AM_Roll[static_cast<int>(EDodgeDirection::Backward)] : AM_Roll[static_cast<int>(EDodgeDirection::Forward)];
	GetAnimInstance()->Montage_Play(PlayedMontage);
	
}

void AMyGameCharacterBase::RollEnd()
{
	IDodgeInterface::RollEnd();

	SetTickRate(1.0f);
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
