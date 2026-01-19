#include "CoreMinimal.h"
#include "AMyGameCharacterBase.h"

#include "Engine/Class/Framework/AGameMode.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Component/Health/UHealthComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/UUltimateBaseComponent.h"
#include "MyGame/Core/AMyGamePlayerController.h"
#include "MyGame/Core/ATownGameMode.h"
#include "Engine/FAudioDevice.h"

#include <algorithm>
#include <cmath>
#include <limits>

XMFLOAT4 GetElementColor(EElementType ElementType)
{
	XMFLOAT3 Color255 = {0,0,0};
	switch (ElementType)
	{
	case EElementType::Fusion:	
		Color255 = XMFLOAT3{198,42,79};
		break;
	case EElementType::Glacio:
		Color255 = {57,176,211};
		break;
	case EElementType::Conducto:
		Color255 = {167, 50, 176};
		break;
	case EElementType::Spectra:
		Color255 = {187,169,29};
		break;
	case EElementType::Aero:
		Color255 = {46,198,158};
		break;
	case EElementType::Havoc:
		Color255 = {151,22,84};
		break;
	}

	return XMFLOAT4{Color255.x/255.0f, Color255.y/255.0f, Color255.z/255.0f,1.0f};
	
}

AMyGameCharacterBase::AMyGameCharacterBase()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = false;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 600;
		CharacterMovement->Braking = 4096;
		CharacterMovement->JumpZVelocity *= 1.5f;
	}

	SpringArm->SetRelativeLocation(XMFLOAT3{0.0f, 50.0f, 0.0f});
	SpringArm->SetArmLength(450);
	

	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});


	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetIsMonochromeObject(false);
	}

	MotionWarpingComponent = std::static_pointer_cast<UMotionWarpingComponent>(CreateDefaultSubobject("MotionWarpingComp", "UMotionWarpingComponent"));
	UltimateSceneCameraComp = std::make_shared<UCameraComponent>();
	HealthComponent = std::static_pointer_cast<UHealthComponent>(CreateDefaultSubobject("HealthComp", "UPlayerHealthComponent"));


	// Player 콜리젼 채널로 변경
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Player);
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Player);
	QueryCheckCapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::AlwaysOverlap, ECollisionResponse::Overlap);
	

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	static constexpr bool bFindCameraRotation = true;
	if (bFindCameraRotation)
	{
		TestComp_CheckCameraPos = std::make_shared<USceneComponent>();
		TestComp_CheckCameraPos->Rename("Hello!!");
		SM_Arrow =  std::make_shared<UStaticMeshComponent>();
		SM_Arrow->SetupAttachment(TestComp_CheckCameraPos);
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

void AMyGameCharacterBase::Register()
{
	LoadCharacterData_OnRegister();
	if (!DeathMontageName.empty())
	{
		AssetManager::GetAsyncAssetCache(DeathMontageName, [this](std::shared_ptr<UObject> Object)
			{
				AM_Death = std::dynamic_pointer_cast<UAnimMontage>(Object);
			});
	}

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
			OnSkeletalMeshLoadSuccess();
		});
	SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceName);


}


void AMyGameCharacterBase::BeginPlay()
{
	ACharacter::BeginPlay();


	HealthComponent->SetMaxHealth(CharacterMaxHealth, true);


	CapsuleComp->SetCollisionObjectType(ECollisionChannel::Player);
	CapsuleComp->SetObjectType(ECollisionChannel::Player);
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Player);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Player);
	
}

void AMyGameCharacterBase::Jump()
{
	ACharacter::Jump();
	PlaySound2DByName("SB_SFX_Jump");
}

void AMyGameCharacterBase::Landing()
{
	ACharacter::Landing();
	PlaySound2DByName("SB_SFX_Land");
}

void AMyGameCharacterBase::Tick_Editor(float DeltaSeconds)
{
	ACharacter::Tick_Editor(DeltaSeconds);

	if (GetRootComponent())
	{
		GetRootComponent()->UpdateComponentToWorld();
	}
}

void AMyGameCharacterBase::BindKeyInputs()
{
	if (APlayerController* PC = dynamic_cast<APlayerController*>(Controller))
	{
		if (std::shared_ptr<UPlayerInput> InputSystem = PC->GetPlayerInput())
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
			InputSystem->BindAction(EKeys::LCtrl, ETriggerEvent::Started, this, &AMyGameCharacterBase::AltButtonPressed);
			InputSystem->BindAction(EKeys::LCtrl, ETriggerEvent::Released, this, &AMyGameCharacterBase::AltButtonReleased);

			// TargetArmLength Wheel
			InputSystem->BindAction(EKeys::MouseWheelUp, Started, this, &AMyGameCharacterBase::WheelUp);
			InputSystem->BindAction(EKeys::MouseWheelDown, Started, this, &AMyGameCharacterBase::WheelDown);

			InputSystem->BindAction(EKeys::F1, Started, this, &AMyGameCharacterBase::Debug_SetUltimateGauge100);
			InputSystem->BindAction(EKeys::F5, Started, this, &AMyGameCharacterBase::Debug_AddGold1000);

			InputSystem->BindAction(EKeys::Tab, Started, this, &AMyGameCharacterBase::ChangeCharacter);

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

void AMyGameCharacterBase::OnSkeletalMeshLoadSuccess()
{
	if (GetRootComponent())
	{
		GetRootComponent()->UpdateComponentToWorld();
	}
}

bool AMyGameCharacterBase::ApplyDamageToEnemy_Range(const FAttackData& AttackData,const std::string& DamageType)
{
	const XMFLOAT3& AttackRange = AttackData.AttackRange;
	std::vector<AActor*> DamagedActors;
	if (AttackData.bIsAttackCenterFixed)
	{
		GPhysicsEngine->BoxOverlapComponents(AttackData.AttackCenterPos, AttackRange, {ECollisionChannel::Enemy}, {},DamagedActors);	
	}
	else
	{
		const XMFLOAT3& ActorLocation = GetActorLocation();
		const XMFLOAT3& ActorForwardVector = GetActorForwardVector();
		XMFLOAT3 BoxPos = ActorLocation + ActorForwardVector * AttackRange/2;
		GPhysicsEngine->BoxOverlapComponents(BoxPos, AttackRange, {ECollisionChannel::Enemy}, {},DamagedActors);	
	}
	

	if (!DamagedActors.empty())
	{
		FMyGameDamageEvent Event;
		Event.ElementType = EElementType::Aero;
		Event.DamageType = DamageType;
		for (size_t i = 0; i < DamagedActors.size(); ++i)
		{
			DamagedActors[i]->TakeDamage(GetCurrentPower() * AttackData.DamagePercent, Event, this);
		}

		if (UltimateComponent)
		{
			UltimateComponent->AddUltimateGauge(AttackData.GainUltimateGauge * static_cast<float>(DamagedActors.size()));
		}

		if (DamageType == "SH_BasicAttack")
		{
			PlaySound2DRandom({"SB_SFX_Attack_Hit_01", "SB_SFX_Attack_Hit_02"});
		}
		else if (DamageType == "SH_SkillAttack")
		{
			PlaySound2DByName("SB_SFX_Magic_Ice");
		}
		else if (DamageType == "SH_UltAttack")
		{
			PlaySound2DByName("SB_SFX_Magic_Ultimate");
		}
		else if (DamageType == "BreakIce")
		{
			PlaySound2DByName("SB_SFX_Explosion_Small");
		}
	}

	return !DamagedActors.empty();
}

void AMyGameCharacterBase::ApplyDamageToEnemy(AActor* DamagedEnemy, const FAttackData& AttackData, const std::string& DamageType)
{
	FMyGameDamageEvent Event;
	Event.ElementType = EElementType::Aero;
	Event.DamageType = DamageType;
	DamagedEnemy->TakeDamage(GetCurrentPower() * AttackData.DamagePercent, Event, this);

	if (UltimateComponent)
	{
		UltimateComponent->AddUltimateGauge(AttackData.GainUltimateGauge);
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
	PlaySound2DByName("SB_SFX_UI_Toggle");
}

void AMyGameCharacterBase::ChangeToNormalCamera(float BlendTime)
{
	GEngine->GetWorld()->GetCameraManager()->SetViewTargetWithBlend(CameraComp, BlendTime, EViewTargetBlendFunction::Cubic);
	PlaySound2DByName("SB_SFX_UI_Toggle");
}


float AMyGameCharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	
	if (bIsDodging)
	{
		ChangeToRoll();
		return DamageAmount;
	}

	DamageAmount = max(0, DamageAmount - GetArmorPower());
	const float RemainingHealth = HealthComponent->ApplyDamage(DamageAmount);
	if (RemainingHealth <= 0.0f)
	{
		Death();	
	}
	else if (DamageAmount > 0.0f)
	{
		PlaySound2DByName("SB_SFX_Player_Hit");
	}

	// 대미지 폰트 띄우기
	if (AMyGamePlayerController* PC = dynamic_cast<AMyGamePlayerController*>(GetWorld()->GetPlayerController()))
	{
		XMFLOAT4 DefaultColor = {0,0,0,1};
		if (const FMyGameDamageEvent* MyGameDamageEvent = dynamic_cast<const FMyGameDamageEvent*>(&DamageEvent))
		{
			DefaultColor = GetElementColor(MyGameDamageEvent->ElementType);
		}
		PC->SpawnFloatingDamage(GetRootComponent()->GetComponentTransform(), DefaultColor, DamageAmount);
	}

	return DamageAmount;
}

void AMyGameCharacterBase::Death()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	PlaySound2DByName("SB_SFX_Player_Death");

	if (const std::shared_ptr<UAnimInstance>& AnimInstance = GetAnimInstance())
	{
		if (AM_Death)
		{
			AnimInstance->Montage_Play(AM_Death, 0);
		}
	}

	MY_LOG("Death", EDebugLogLevel::DLL_Warning, "Character Death");

	if (const std::shared_ptr<AGameMode>& GameMode = GetWorld()->GetPersistentLevel()->GetGameMode())
	{
		if (const std::shared_ptr<ADungeonGameMode>& DungeonGameMode = std::dynamic_pointer_cast<ADungeonGameMode>(GameMode))
		{
			GEngine->SetInputMode(EInputMode::InputMode_UIOnly);
			GEngine->SetMouseLock(EMouseLockMode::DoNotLock);
			GEngine->ShowCursor(true);
			DungeonGameMode->HandlePlayerDeath();
		}
	}

	
}

void AMyGameCharacterBase::Move(float X, float Y)
{
	XMFLOAT3 ForwardDirection = Controller->GetActorForwardVector();
	XMVECTOR ForwardDirVec = XMVectorSet(ForwardDirection.x,0.0f,ForwardDirection.z, 0.0f);
	ForwardDirVec = XMVector3NormalizeEst(ForwardDirVec);
	XMStoreFloat3(&ForwardDirection, ForwardDirVec);

	XMFLOAT3 RightDirection = Controller->GetActorRightVector();

	AddMovementInput(ForwardDirection, X);
	AddMovementInput(RightDirection, Y);
}

void AMyGameCharacterBase::Look(float X, float Y)
{
	if (APlayerController* PC = dynamic_cast<APlayerController*>(Controller))
	{
		if (!bAltButtonPressed && PC->GetPlayerInput())
		{
			XMFLOAT2 Delta = PC->GetPlayerInput()->LastMouseDelta;
			AddControllerYawInput(Delta.x);
			AddControllerPitchInput(Delta.y);
			SpringArm->TickComponent(0.0f);
		}
	}
}

void AMyGameCharacterBase::AltButtonPressed()
{
	bAltButtonPressed = true;
	GEngine->SetInputMode(EInputMode::InputMode_GameUI);
	GEngine->SetMouseLock(EMouseLockMode::DoNotLock);
	GEngine->ShowCursor(true);
}

void AMyGameCharacterBase::AltButtonReleased()
{
	bAltButtonPressed = false;
	GEngine->SetInputMode(EInputMode::InputMode_GameOnly);
	GEngine->SetMouseLock(EMouseLockMode::LockAlways);
	GEngine->ShowCursor(false);
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
	PlaySound2DByName("SB_SFX_Dodge");
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
	PlaySound2DByName("SB_SFX_Roll");
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

void AMyGameCharacterBase::ChangeCharacter()
{
	std::vector<std::shared_ptr<AActor>> Actors;
	if (GetClass() == "AGideonCharacter")
	{
		Actors = GetWorld()->GetPersistentLevel()->GetActorsFromClass("ASanhwaCharacter");
	}
	else
	{
		Actors = GetWorld()->GetPersistentLevel()->GetActorsFromClass("AGideonCharacter");
	}

	if (!Actors.empty())
	{
		if (APlayerController* PC = GetWorld()->GetPlayerController())
		{
			if (const std::shared_ptr<ACharacter>& OtherCharacter = std::dynamic_pointer_cast<ACharacter>(Actors[0]))
			{
				XMFLOAT3 OriginPosition = GetActorLocation();
				SetActorLocation(XMFLOAT3{0,0,0});	
				OtherCharacter->SetActorLocation(OriginPosition);
				PC->OnPossess(OtherCharacter.get());
				PlaySound2DByName("SB_SFX_Character_Switch");
			}
			
		}
	}
}

void AMyGameCharacterBase::ApplyPotion()
{
}

void AMyGameCharacterBase::Debug_SetUltimateGauge100()
{
	if (UltimateComponent)
	{
		UltimateComponent->AddUltimateGauge(100000);
	}
}

void AMyGameCharacterBase::Debug_AddGold1000()
{
	UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>();
	if (!GameInstance)
	{
		return;
	}

	if (!GameInstance->AddGold(1000))
	{
		return;
	}

	if (const std::shared_ptr<ATownGameMode>& TownGameMode = std::dynamic_pointer_cast<ATownGameMode>(GetWorld()->GetPersistentLevel()->GetGameMode()))
	{
		TownGameMode->RefreshEquipmentStatusWidget();
	}
}

float AMyGameCharacterBase::GetCurrentPower() const
{
	return MyMath::FRandRange(MinPower, MaxPower) * GetWeaponPower();
}

float AMyGameCharacterBase::GetWeaponPower() const
{
	// 레벨당 30%로 간단히 적용
	return 1.0f + 0.3f * EquipLevel[static_cast<int>(EEquipType::Weapon)];
}

float AMyGameCharacterBase::GetArmorPower() const
{
	// 레벨당 100씩 막아줌
	return 100.0f * (EquipLevel[static_cast<int>(EEquipType::Armor)] + EquipLevel[static_cast<int>(EEquipType::Glove)] + EquipLevel[static_cast<int>(EEquipType::Head)]);
}

void AMyGameCharacterBase::SetEquipmentLevel(const std::array<int, static_cast<int>(EEquipType::Count)>& NewEquipArr)
{
	for (int i = 0 ; i < static_cast<int>(EEquipType::Count); ++i)
	{
		EquipLevel[i] = NewEquipArr[i];	
	}
	
}

void AMyGameCharacterBase::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);


}
