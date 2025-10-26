#include "CoreMinimal.h"
#include "AGideonCharacter.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Actor/Gideon/AGideonFireBall.h"
#include "MyGame/Actor/Gideon/AGideonIceStorm.h"
#include "MyGame/Actor/Sanhwa/ASanhwaIceBase.h"
#include "MyGame/Component/Combat/Range/UGideonCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/Gideon/UGideonSkillComponent.h"
#include "MyGame/Widget/Gideon/UGideonWidget.h"

std::string AGideonCharacter::CharacterName  = "Gideon";

AGideonCharacter::AGideonCharacter()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = false;
		CharacterMovement->Acceleration = 2048.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 3000.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 550.0;
		CharacterMovement->Braking = 1024;

		// 캐릭터가 조준 모드 (bOrientRotationToMovement == true) 일 때 Yaw 에 맞춰 회전되도록
		CharacterMovement->bUseControllerRotationPitch = false;
		CharacterMovement->bUseControllerRotationYaw = true;
		CharacterMovement->bUseControllerRotationRoll = false;
	}

	CharacterMeshName = "SK_Gideon";
	AnimInstanceName = "UGideonAnimInstance";

	//DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AboxM_UE5MM_Dodge_Fwd";
	//DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	//RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	//RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";

	CombatComponent = std::dynamic_pointer_cast<UGideonCombatComponent>(CreateDefaultSubobject("GideonCombatComp", "UGideonCombatComponent"));
	SkillComponent = std::dynamic_pointer_cast<UGideonSkillComponent>(CreateDefaultSubobject("GideonSkillComp", "UGideonSkillComponent"));
	//UltimateComponent = std::dynamic_pointer_cast<USanhwaUltimateComponent>(CreateDefaultSubobject("SanhwaUltComp", "USanhwaUltimateComponent"));

	CharacterMaxHealth = 20000.0f;

	ElementType = EElementType::Fusion;

	AimModeSpringArm = std::make_shared<USpringArmComponent>();
	AimModeSpringArm->SetupAttachment(GetRootComponent());
	AimModeSpringArm->SetRelativeLocation({75.0f, 75.0f,0.0f});
	AimModeSpringArm->SetArmLength(300.0f);
	AimModeSpringArm->SetCheckCollision(false);

	AimModeCameraComp = std::make_shared<UCameraComponent>();
	AimModeCameraComp->SetupAttachment(AimModeSpringArm);
	
}

void AGideonCharacter::Register()
{
	//UltimateSceneCameraComp->SetupAttachment(SkeletalMeshComponent, "spine_05");
	//UltimateSceneCameraComp->SetRelativeLocation({11.6f,167.28f,0.0f});
	//UltimateSceneCameraComp->SetRelativeRotation(XMFLOAT4{-0.073f,-0.704f,0.078f, 0.702f});
	//if (TestComp_DeleteLater)
	//{
	//	TestComp_DeleteLater->SetupAttachment(SkeletalMeshComponent, "spine_05");
	//	TestComp_DeleteLater->SetRelativeLocation({11.6f,167.28f,0.0f});
	//	TestComp_DeleteLater->SetRelativeRotation(XMFLOAT4{-0.073f,-0.704f,0.078f, 0.702f});
	//}
	
	AMyGameCharacterBase::Register();

	
}

void AGideonCharacter::Tick(float DeltaSeconds)
{
	AMyGameCharacterBase::Tick(DeltaSeconds);
}

void AGideonCharacter::BindKeyInputs()
{
	AMyGameCharacterBase::BindKeyInputs();

	if (APlayerController* PC = dynamic_cast<APlayerController*>(Controller))
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = PC->GetPlayerInput())
		{
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Started, this, &AGideonCharacter::ToAimMode);
			InputSystem->BindAction(EKeys::MouseRight, ETriggerEvent::Released, this, &AGideonCharacter::ToNormalMode);
		}
	}
}


void AGideonCharacter::SpawnFireBall(const FTransform& SpawnTransform, const FAttackData& AttackData, const XMFLOAT3& TargetPosition)
{
	if (std::shared_ptr<AGideonFireBall> FireBall = std::dynamic_pointer_cast<AGideonFireBall>(GetWorld()->SpawnActor("AGideonFireBall", SpawnTransform)))
	{
		FireBall->Initialize(this, TargetPosition, AttackData);
	}
}

void AGideonCharacter::SpawnIceStorm(const FTransform& SpawnTransform, const FAttackData& AttackData)
{
	if (std::shared_ptr<AGideonIceStorm> IceStorm = std::dynamic_pointer_cast<AGideonIceStorm>(GetWorld()->SpawnActor("AGideonIceStorm", SpawnTransform)))
	{
		IceStorm->Initialize(this, AttackData);
	}
}

void AGideonCharacter::CreateWidgetOnBeginPlay()
{
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		std::shared_ptr<UGideonWidget> GideonWidget = std::make_shared<UGideonWidget>();
		PC->CreateWidget(CharacterName, GideonWidget);
		CharacterWidget = GideonWidget;
	}
}

void AGideonCharacter::Look(float X, float Y)
{
	AMyGameCharacterBase::Look(X , Y);
	APlayerController* PC = dynamic_cast<APlayerController*>(Controller);
	if (PC && bIsAimMode)
	{
		XMFLOAT2 Delta = PC->GetPlayerInput()->LastMouseDelta;

		XMFLOAT4 Rot = AimModeSpringArm->GetWorldRotation();
		XMVECTOR ControlRotation = XMLoadFloat4(&Rot);
		XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), ControlRotation));
		XMVECTOR UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

		XMVECTOR ForwardXZ = XMVector3Normalize(XMVectorSet(XMVectorGetX(ForwardVector), 0.0f, XMVectorGetZ(ForwardVector), 0.0f));
		float Dot = XMVectorGetX(XMVector3Dot(ForwardVector, ForwardXZ));
		Dot = std::clamp(Dot, -1.0f, 1.0f);
		float CurrentAngleRad = acosf(Dot);
		if (XMVectorGetY(ForwardVector) > 0)
			CurrentAngleRad *= -1;

		float DesiredPitchDeg = XMConvertToDegrees(CurrentAngleRad) + Delta.y;
		float ClampedPitchDeg = std::clamp(DesiredPitchDeg, AimModePitchMin, AimModePitchMax);

		XMVECTOR PitchQuat = XMQuaternionRotationAxis(RightVector, ClampedPitchDeg);
		AimModeSpringArm->SetWorldRotation(PitchQuat);
		AimModeSpringArm->TickComponent(0.0f);
	}
}

void AGideonCharacter::ToAimMode()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->MaxWalkSpeed = 150.0f;
		Movement->Braking = 128.0f;
	}
	GetWorld()->GetCameraManager()->SetViewTargetWithBlend(AimModeCameraComp, 0.5f, EViewTargetBlendFunction::Linear);
	bIsAimMode = true;

	if (const std::shared_ptr<UGideonWidget>& GideonWidget = std::dynamic_pointer_cast<UGideonWidget>(CharacterWidget.lock()))
	{
		GideonWidget->SetAimModeWidgetVisibility(true);
	}

	AimModeSpringArm->SetRelativeRotation(XMFLOAT4{0,0,0,1});
}

void AGideonCharacter::ToNormalMode()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 550.0f;
		Movement->bOrientRotationToMovement = false;
		Movement->Braking = 1024;
	}
	GetWorld()->GetCameraManager()->SetViewTargetWithBlend(CameraComp, 0.5f, EViewTargetBlendFunction::Linear);
	bIsAimMode = false;

	if (const std::shared_ptr<UGideonWidget>& GideonWidget = std::dynamic_pointer_cast<UGideonWidget>(CharacterWidget.lock()))
	{
		GideonWidget->SetAimModeWidgetVisibility(false);
	}
}


/*

void ASanhwaCharacter::BeginPlay()
{
	AMyGameCharacterBase::BeginPlay();

}

void ASanhwaCharacter::Tick(float DeltaSeconds)
{
	AMyGameCharacterBase::Tick(DeltaSeconds);

	static float LastTime = GEngine->GetTimeSeconds();
	if (LastTime + 3.0f <= GEngine->GetTimeSeconds())
	{
		LastTime = GEngine->GetTimeSeconds();
		TakeDamage(2500.0f, {},this);	
	}
	
}

void ASanhwaCharacter::CreateIceSpikes(bool bIsUltimate)
{
	XMFLOAT3 ActorLocation = GetActorLocation();
	XMFLOAT3 ActorForwardVector = GetActorForwardVector();


	FTransform SpawnTransform;
	SpawnTransform.Translation = ActorLocation + ActorForwardVector * 100;
	SpawnTransform.Scale3D = {1,1,1};
	SpawnTransform.Rotation = MyMath::VectorToRotationQuaternion(ActorForwardVector);

	std::shared_ptr<AActor> IceSpikesActor;
	if (bIsUltimate)
	{
		IceSpikesActor = GetWorld()->SpawnActor("ASanhwaIceSpikeUltimate", SpawnTransform);
	}
	else
	{
		IceSpikesActor = GetWorld()->SpawnActor("ASanhwaIceSpikeBase", SpawnTransform);	
	}

	if (const std::shared_ptr<ASanhwaIceSpikeBase>& IceActor = std::dynamic_pointer_cast<ASanhwaIceSpikeBase>(IceSpikesActor))
	{
		IceActor->SpawnedBy(this);
	}
}*/
