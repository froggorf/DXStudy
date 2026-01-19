#include "CoreMinimal.h"
#include "AEnemyBase.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UWidgetComponent.h"
#include "Engine/World/UWorld.h"
#include "MyGame/AnimInstance/Animal/UEnemyAnimInstance.h"
#include "MyGame/BehaviorTree/TestBT.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Component/Health/UHealthComponent.h"
#include "Engine/FAudioDevice.h"
#include "MyGame/Core/UMyGameInstance.h"
#include "MyGame/Core/AMyGamePlayerController.h"
#include "MyGame/Widget/Health/UEnemyHealthWidget.h"
#include "MyGame/Widget/Health/UHealthWidgetBase.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
	UINT ToDisplayDamageValue(float DamageAmount)
	{
		if (!std::isfinite(DamageAmount))
		{
			return UINT_MAX;
		}
		if (DamageAmount <= 0.0f)
		{
			return 0;
		}

		const float MaxDisplay = static_cast<float>(UINT_MAX);
		const float Clamped = std::min(DamageAmount, MaxDisplay);
		return static_cast<UINT>(max(1.0f, std::round(Clamped)));
	}

}

static UINT EnemyCount = 0;
AEnemyBase::AEnemyBase()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = false;
		CharacterMovement->Acceleration = 4096.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 450;
		CharacterMovement->Braking = 4096;
	}

	CapsuleComp->SetHalfHeight(12.5f);
	CapsuleComp->SetRadius(50.0f);
	CapsuleComp->SetObjectType(ECollisionChannel::Enemy);
	SkeletalMeshComponent->SetRelativeLocation({0,-12.5f*5,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});
	
	MotionWarpingComponent = std::static_pointer_cast<UMotionWarpingComponent>(CreateDefaultSubobject("MotionWarpingComp", "UMotionWarpingComponent"));
	HealthComponent = std::static_pointer_cast<UHealthComponent>(CreateDefaultSubobject("HealthComp", "UHealthComponent"));

	TempStaticMesh = std::make_shared<UStaticMeshComponent>();
	TempStaticMesh->SetupAttachment(GetRootComponent());
	TempStaticMesh->SetRelativeScale3D({10,10,10});

	HealthWidgetComp = std::make_shared<UWidgetComponent>();
	HealthWidgetComp->SetupAttachment(GetRootComponent());
	HealthWidgetComp->SetRelativeLocation({0.0f,90.0f,0.0f});
}

void AEnemyBase::Register()
{
	Rename("Enemy_" + std::to_string(EnemyCount++));

	ACharacter::Register();

	AssetManager::GetAsyncAssetCache(SkeletalMeshName,[this](std::shared_ptr<UObject> Object)
		{
			SkeletalMeshComponent->SetSkeletalMesh(std::dynamic_pointer_cast<USkeletalMesh>(Object));
		});
	SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceName);

	/*AssetManager::GetAsyncAssetCache("SM_DeferredSphere",[this](std::shared_ptr<UObject> Object)
		{
			TempStaticMesh->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			TempStaticMesh->SetCollisionObjectType(ECollisionChannel::Enemy);
			TempStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		});*/

	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Enemy);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Enemy);

	std::shared_ptr<UHealthWidgetBase> EnemyHealthWidget = std::make_shared<UEnemyHealthWidget>();
	HealthWidgetComp->SetWidget(EnemyHealthWidget);
	HealthComponent->SetHealthWidget(EnemyHealthWidget);

	EnemyAnimInstance = std::dynamic_pointer_cast<UEnemyAnimInstanceBase>(GetAnimInstance());
	
	
}

void AEnemyBase::BeginPlay()
{
	ACharacter::BeginPlay();

	AIController = std::dynamic_pointer_cast<AAIController>(GetWorld()->SpawnActor("AAIController", FTransform{}));
	BindingBehaviorTree();

	// Enemy 콜리젼 채널로 변경
	CapsuleComp->Rename("EnemyCapsuleComp");
	CapsuleComp->SetCollisionObjectType(ECollisionChannel::Enemy);
	CapsuleComp->SetObjectType(ECollisionChannel::Enemy);
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Enemy);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Enemy);

	if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
	{
		const UINT Stage = max(1u, GameInstance->GetStageLevel());
		if (Stage > 1)
		{
			float StageScale = std::pow(2.0f, static_cast<float>(Stage - 1));
			const float MaxPowerScale = FLT_MAX / max(1.0f, EnemyPower);
			const float MaxHealthScale = FLT_MAX / max(1.0f, EnemyMaxHealth);
			const float MaxScale = std::min(MaxPowerScale, MaxHealthScale);
			if (!std::isfinite(StageScale) || StageScale > MaxScale)
			{
				StageScale = MaxScale;
			}
			EnemyPower *= StageScale;
			EnemyMaxHealth *= StageScale;
		}
	}

	HealthComponent->SetMaxHealth(EnemyMaxHealth, true);



}

float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	if (HealthComponent->GetHealthPercent() <= 0.0f)
	{
		return DamageAmount;
	}

	if (HealthComponent->ApplyDamage(DamageAmount) <=0.0f)
	{
		Death();
	}
	else
	{
		if (AIController)
		{
			float NewHealthPercent = HealthComponent->GetHealthPercent() * 100;
			AIController->GetBehaviorTree()->GetBlackBoard()->SetValue<FBlackBoardValueType_FLOAT>("HealthPercent", NewHealthPercent);
		}
		if (DamageAmount > 0.0f)
		{
			PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Attack_Hit_02");
		}
	}

	if (AMyGamePlayerController* PC = dynamic_cast<AMyGamePlayerController*>(GetWorld()->GetPlayerController()))
	{
		XMFLOAT4 DefaultColor = {0,0,0,1};
		if (const FMyGameDamageEvent* MyGameDamageEvent = dynamic_cast<const FMyGameDamageEvent*>(&DamageEvent))
		{
			DefaultColor = GetElementColor(MyGameDamageEvent->ElementType);
		}
		PC->SpawnFloatingDamage(GetRootComponent()->GetComponentTransform(), DefaultColor, ToDisplayDamageValue(DamageAmount));
	}

	return DamageAmount;
}

void AEnemyBase::PlayAttackMontage(Delegate<> AttackFinishDelegate)
{
	bBasicAttackApplied = false;
	if (EnemyAnimInstance)
	{
		PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Enemy_Attack");
		EnemyAnimInstance->DoAttackAnim(AttackFinishDelegate);
	}
	else
	{
		MY_LOG("AEnemyBase",EDebugLogLevel::DLL_Warning,"No Valid EnemyAnimInstance");
	}

}

void AEnemyBase::ApplyBasicAttack()
{
	if (bBasicAttackApplied)
	{
		return;
	}
	bBasicAttackApplied = true;

	XMFLOAT3 ForwardDir = GetActorForwardVector();
	XMFLOAT3 ActorLocation = GetActorLocation();
	float CapsuleRadius = GetCapsuleComponent()->GetRadius();
	XMFLOAT3 AttackBoxCenter = ActorLocation + ForwardDir * (CapsuleRadius + BasicAttackData.AttackRange.z);

	FMyGameDamageEvent DamageEvent;
	DamageEvent.ElementType = EElementType::Spectra;

	std::vector<AActor*> OverlapResults;
	GPhysicsEngine->BoxOverlapComponents(AttackBoxCenter, BasicAttackData.AttackRange, {ECollisionChannel::Player}, {}, OverlapResults);
	for (AActor* OverlapActor : OverlapResults)
	{
		if (AMyGameCharacterBase* Player =  dynamic_cast<AMyGameCharacterBase*>(OverlapActor))
		{
			Player->TakeDamage(EnemyPower * BasicAttackData.DamagePercent, DamageEvent, this);	
		}
	}

}


void AEnemyBase::Death()
{
	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Enemy_Death");
	OnDeath.Broadcast();

	if (const std::shared_ptr<UAnimInstance>& AnimInstance = GetAnimInstance())
	{
		AnimInstance->Montage_Play(GetDeathAnimMontage(), 0,  {this, &AEnemyBase::DestroyAtDeath});
	}
	if (AIController)
	{
		AIController->SetAIActivate(false);
	}

	GetWorld()->SpawnActor("ACoin", FTransform{GetActorLocation()});
}

void AEnemyBase::DestroyAtDeath()
{
	DestroySelf();
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		if (const std::shared_ptr<AActor>& PlayerCharacter = PC->GetPlayerCharacter())
		{
			const std::shared_ptr<FBlackBoard>& BlackBoard = AIController->GetBehaviorTree()->GetBlackBoard();
			BlackBoard->SetValue<FBlackBoardValueType_Object>("Player", PlayerCharacter);
			float Distance = MyMath::GetDistance(GetActorLocation(), PlayerCharacter->GetActorLocation());
			bool NewValue = Distance > DetectRange;
			BlackBoard->SetValue<FBlackBoardValueType_Bool>("MoveMode", NewValue);		
		}
	}
	
}

void AEnemyBase::BindingBehaviorTree()
{
	if (AIController)
	{
		AIController->OnPossess(this);
		std::shared_ptr<UTestBT> BT = std::make_shared<UTestBT>();
		BT->Register();
		BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Owner", shared_from_this());

		AIController->SetBehaviorTree(BT);

		AIController->GetBehaviorTree()->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Player", GetWorld()->GetPlayerController()->GetPlayerCharacter());		
	}
}

AWolf::AWolf()
{
	SkeletalMeshName = "SK_Wolf";
	AnimInstanceName = "UWolfAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100, 30, 100}, 1.0f, 0.0f, 0.0f, true};
	EnemyPower = 75.0f;
}

void AWolf::Register()
{
	AEnemyBase::Register();

	AssetManager::GetAsyncAssetCache("AM_Wolf_Death",[this](std::shared_ptr<UObject> Object)
		{
			DeathMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});
}

APig::APig()
{
	SkeletalMeshName = "SK_Pig";
	AnimInstanceName = "UPigAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100,30,100}, 1.0f, 0.0f, 0.0f, true};
}

void APig::Register()
{
	AEnemyBase::Register();

	AssetManager::GetAsyncAssetCache("AM_Pig_Death",[this](std::shared_ptr<UObject> Object)
		{
			DeathMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});
}

AScarecrow::AScarecrow()
{
	SkeletalMeshName = "SK_Wolf";
	AnimInstanceName = "UWolfAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100, 30, 100}, 1.0f, 0.0f, 0.0f, true};
	EnemyPower = 0.0f;
}

void AScarecrow::Register()
{
	AEnemyBase::Register();

	AssetManager::GetAsyncAssetCache("AM_Wolf_Death",[this](std::shared_ptr<UObject> Object)
		{
			DeathMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});
}

void AScarecrow::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void AScarecrow::BindingBehaviorTree()
{
}

ADragon::ADragon()
{
	CapsuleComp->SetHalfHeight(62.5f);
	CapsuleComp->SetRadius(250.0f);

	SkeletalMeshComponent->SetRelativeLocation({0,-62.5f*5,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});
	SkeletalMeshComponent->SetRelativeScale3D({0.5f,0.5f,0.5f});

	SkeletalMeshName = "SK_Dragon";
	AnimInstanceName = "UDragonAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{1000,300,1000}, 1.0f, 0.0f, 0.0f, true};
	EnemyPower = 150.0f;
	DetectRange = 2500.0f;

	FlameVFX = std::make_shared<UNiagaraComponent>();
	FlameVFX->SetupAttachment(SkeletalMeshComponent, FlameSocketName);
	FlameVFX->SetRelativeRotation(FlameVFXRotation);
	FlameVFX->Deactivate();
}

void ADragon::Register()
{
	AEnemyBase::Register();

	AssetManager::GetAsyncAssetCache("NS_DragonFlame",[this](std::shared_ptr<UObject> Object)
		{
			FlameVFXNormalSystem = std::dynamic_pointer_cast<UNiagaraSystem>(Object);
			SetFlameVFXSystem(FlameVFXNormalSystem);
			if (bFlameVFXPendingActivate)
			{
				ActivateFlameVFX();
			}
		});

	AssetManager::GetAsyncAssetCache("NS_DragonFlame_Long",[this](std::shared_ptr<UObject> Object)
		{
			FlameVFXHPSkillSystem = std::dynamic_pointer_cast<UNiagaraSystem>(Object);
		});


	AssetManager::GetAsyncAssetCache("AM_Dragon_Scream",[this](std::shared_ptr<UObject> Object)
		{
			AM_Dragon_Scream = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});

	AssetManager::GetAsyncAssetCache("AM_Dragon_Death",[this](std::shared_ptr<UObject> Object)
		{
			DeathMontage = std::dynamic_pointer_cast<UAnimMontage>(Object);
		});
}

void ADragon::BindingBehaviorTree()
{
	if (AIController)
	{
		AIController->OnPossess(this);
		std::shared_ptr<UDragonBT> BT = std::make_shared<UDragonBT>();
		AIController->SetBehaviorTree(BT);
		BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Owner", shared_from_this());
		BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Player", GetWorld()->GetPlayerController()->GetPlayerCharacter());


		AssetManager::GetAsyncAssetCache("AM_Dragon_Flame",[this](std::shared_ptr<UObject> Object)
			{
				AM_Dragon_Flame = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (AIController)
				{
					if (const std::shared_ptr<UBehaviorTree>& BT = AIController->GetBehaviorTree())
					{
						BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("FlameAnim",AM_Dragon_Flame);
					}
				}
			});

		AssetManager::GetAsyncAssetCache("AM_Dragon_StartFly",[this](std::shared_ptr<UObject> Object)
			{
				AM_Dragon_FlyStart = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (AIController)
				{
					if (const std::shared_ptr<UBehaviorTree>& BT = AIController->GetBehaviorTree())
					{
						BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("StartFlyAnim",AM_Dragon_FlyStart);
					}
				}
			});

		AssetManager::GetAsyncAssetCache("AM_Dragon_Land",[this](std::shared_ptr<UObject> Object)
			{
				AM_Dragon_Landing = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (AIController)
				{
					if (const std::shared_ptr<UBehaviorTree>& BT = AIController->GetBehaviorTree())
					{
						BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("LandAnim",AM_Dragon_Landing);
					}
				}
			});

		AssetManager::GetAsyncAssetCache("AM_Dragon_HPFlame",[this](std::shared_ptr<UObject> Object)
			{
				AM_Dragon_HPFlame = std::dynamic_pointer_cast<UAnimMontage>(Object);
				if (AIController)
				{
					if (const std::shared_ptr<UBehaviorTree>& BT = AIController->GetBehaviorTree())
					{
						BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("HPFlame",AM_Dragon_HPFlame);
					}
				}
			});
	}
}

bool ADragon::StartFlameSkillCharge()
{
	if (!AM_Dragon_Scream || !AM_Dragon_Flame)
	{
		//MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning, "No valid Dragon Skill")
		return false;
	}

	//MY_LOG("LOG", EDebugLogLevel::DLL_Warning, "Dragon charges skill")

	// TODO: 스킬 차지용 애니메이션이나 이펙트 구현 할 시 이곳에서 사용
	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Dragon_Roar");
	if (const std::shared_ptr<UAnimInstance>& AnimInstance = GetAnimInstance())
	{
		AnimInstance->Montage_Play(AM_Dragon_Scream, 0);
	}

	FTransform SpawnTransform;
	SpawnTransform.Translation = GetActorLocation();
	float Radius = GetFlameSkillRadius() * 2;
	SpawnTransform.Scale3D = XMFLOAT3 {Radius, 10.0f, Radius};
	static std::vector<ECollisionChannel> CollisionChannels;
	if (CollisionChannels.empty())
	{
		CollisionChannels.reserve(static_cast<int>(ECollisionChannel::Count));
		for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			if (Channel == ECollisionChannel::Player || Channel == ECollisionChannel::Enemy)
			{
				continue;
			}
			CollisionChannels.emplace_back(Channel);
		}
	}
	FHitResult HitResult;
	XMFLOAT3 Start = SpawnTransform.Translation;
	// 약간의 갭을 주기 위하여
	Start.y += -10.0f;
	XMFLOAT3 End = Start;
	End.y -= 1000.0f;
	if (GPhysicsEngine->LineTraceSingleByChannel(Start, End, CollisionChannels, HitResult))
	{
		MY_LOG("LOG",EDebugLogLevel::DLL_Error,HitResult.HitComponent->GetName() );
		MY_LOG("LOG",EDebugLogLevel::DLL_Error,HitResult.HitComponent->GetClass() ); 
		SpawnTransform.Translation = HitResult.Location;
		MY_LOG("LOG",EDebugLogLevel::DLL_Error,XMFLOAT3_TO_TEXT(HitResult.Location));
	}

	if (!ShowRangeActor)
	{
		ShowRangeActor = std::dynamic_pointer_cast<ARangeDecalActor>(GEngine->GetWorld()->SpawnActor("ARangeDecalActor", SpawnTransform));
	}
	ShowRangeActor->SetForward(GetActorForwardVector())
				->SetHalfAngleDeg(GetFlameSkillHalfAngle());

	return true;
}

void ADragon::ResetSkillRangeActor()
{
	if (ShowRangeActor)
	{
		GetWorld()->GetPersistentLevel()->DestroyActor(ShowRangeActor.get());	
	}
	ShowRangeActor.reset();
}

void ADragon::SkillCharging(float Progress)
{
	if (ShowRangeActor)
	{
		ShowRangeActor->SetProgress(Progress);	
	}
	
}

void ADragon::SetFlameVFXSystem(const std::shared_ptr<UNiagaraSystem>& System)
{
	if (!FlameVFX || !System)
	{
		return;
	}
	if (ActiveFlameVFXSystem == System && bIsFlameVFXReady)
	{
		return;
	}

	FlameVFX->SetNiagaraAsset(System);
	FlameVFX->RegisterSceneProxies();
	ActiveFlameVFXSystem = System;
	bIsFlameVFXReady = true;
}

void ADragon::ActivateFlameVFX()
{
	if (!FlameVFX || !bIsFlameVFXReady)
	{
		bFlameVFXPendingActivate = true;
		return;
	}

	if (!bIsFlameVFXActive)
	{
		FlameVFX->Activate();
		bIsFlameVFXActive = true;
	}
	bFlameVFXPendingActivate = false;
}

void ADragon::DeactivateFlameVFX()
{
	bFlameVFXPendingActivate = false;
	if (FlameVFX && bIsFlameVFXActive)
	{
		FlameVFX->Deactivate();
		bIsFlameVFXActive = false;
	}
}

void ADragon::StartFlame()
{
	// 이펙트 시작
	SetFlameVFXSystem(FlameVFXNormalSystem);
	ActivateFlameVFX();
	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_Fire");

	// 공격 적용하기
	// NOTE: 어차피 플레이어가 1인인 게임이라 따로 충돌체크 없이
	// 플레이어와 거리, 각도를 계산해서 적용
	if (const std::shared_ptr<AActor>& Player = GetWorld()->GetPlayerController()->GetPlayerCharacter())
	{
		// Y축 정보는 취급 안함
		XMFLOAT3 PlayerLocationXZ = Player->GetActorLocation();
		PlayerLocationXZ.y = 0.0f;
		XMFLOAT3 DragonLocationXZ = GetActorLocation();
		DragonLocationXZ.y = 0.0f;
		float Distance = MyMath::GetDistance(PlayerLocationXZ, DragonLocationXZ);
		if (Distance <= FlameSkillRadius)
		{
			XMFLOAT3 ToPlayer = MyMath::GetDirectionUnitVector(GetActorLocation(), Player->GetActorLocation());
			XMFLOAT3 Forward = GetActorForwardVector();
			float Dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&ToPlayer), XMLoadFloat3(&Forward)));

			float AngleToPlayer = std::abs(XMConvertToDegrees(std::acos(Dot)));
			if (AngleToPlayer <= FlameHalfAngleDeg)
			{
				FDamageEvent Event;
				Event.DamageType = "Flame";
				Player->TakeDamage(EnemyPower * FlameSkillAttackPower, Event, this);
			}

		}
	}
}

void ADragon::EndFlame()
{
	// 불 뿜는 이펙트 종료
	DeactivateFlameVFX();
}

bool ADragon::StartHPSkillCharge()
{
	if (!AM_Dragon_HPFlame)
	{
		return false;
	}

	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Dragon_Roar");

	FTransform SpawnTransform;
	XMFLOAT3 Forward = GetActorForwardVector();
	Forward.y = 0.0f;
	XMStoreFloat3(&Forward, XMVector3Normalize(XMLoadFloat3(&Forward)));
	SpawnTransform.Translation = GetActorLocation() + Forward * HPSkillRange.z/2;
	SpawnTransform.Rotation = MyMath::ForwardVectorToRotationQuaternion(Forward);
	SpawnTransform.Scale3D = HPSkillRange;
	static std::vector<ECollisionChannel> CollisionChannels;
	if (CollisionChannels.empty())
	{
		CollisionChannels.reserve(static_cast<int>(ECollisionChannel::Count));
		for (int i = 0; i < static_cast<int>(ECollisionChannel::Count); ++i)
		{
			ECollisionChannel Channel = static_cast<ECollisionChannel>(i);
			if (Channel == ECollisionChannel::Player || Channel == ECollisionChannel::Enemy)
			{
				continue;
			}
			CollisionChannels.emplace_back(Channel);
		}
	}
	FHitResult HitResult;
	XMFLOAT3 Start = SpawnTransform.Translation;
	// 약간의 갭을 주기 위하여
	Start.y += 50.0f;
	XMFLOAT3 End = Start;
	End.y -= 1000.0f;
	if (GPhysicsEngine->LineTraceSingleByChannel(Start, End, CollisionChannels, HitResult))
	{
		SpawnTransform.Translation = HitResult.Location;
	}

	if (ShowRangeActor)
	{
		GetWorld()->GetPersistentLevel()->DestroyActor(ShowRangeActor.get());
		ShowRangeActor = nullptr;
	}

	if (!ShowRangeActor)
	{
		ShowRangeActor = std::dynamic_pointer_cast<ARangeDecalActor>(GEngine->GetWorld()->SpawnActor("ARangeDecalActor", SpawnTransform));
	}
	ShowRangeActor->SetRangeType(ERangeType::Rect);


	return true;
}

void ADragon::StartHPSkill()
{
	if (FlameVFXHPSkillSystem)
	{
		SetFlameVFXSystem(FlameVFXHPSkillSystem);
	}
	else
	{
		SetFlameVFXSystem(FlameVFXNormalSystem);
	}
	ActivateFlameVFX();
	PlaySoundAtLocationByName(GetWorld(), GetActorLocation(), "SB_SFX_Explosion_02");
	XMFLOAT3 DragonPos = GetActorLocation();
	XMFLOAT3 Forward = GetActorForwardVector();
	Forward.y = 0.0f;
	XMStoreFloat3(&Forward, XMVector3Normalize(XMLoadFloat3(&Forward)));

	XMFLOAT4 Rotation = MyMath::ForwardVectorToRotationQuaternion(Forward);

	XMFLOAT3 AttackBoxPos = DragonPos + Forward * HPSkillRange.z/2;
	XMFLOAT3 AttackBoxExtent = {HPSkillRange.x, 1000.0f, HPSkillRange.z};

	std::vector<ECollisionChannel> TargetChannels = { ECollisionChannel::Player };
	std::vector<AActor*> ActorsToIgnore = { this };
	std::vector<AActor*> HitActors;

	GPhysicsEngine->BoxOverlapComponentsWithRotation(
		AttackBoxPos, 
		AttackBoxExtent, 
		Rotation,  
		TargetChannels, 
		ActorsToIgnore, 
		HitActors
	);

	// 데미지 적용
	for (AActor* HitActor : HitActors)
	{
		if (HitActor)
		{
			FDamageEvent Event;
			Event.DamageType = "DragonHPSkill";
			HitActor->TakeDamage(EnemyPower * 100, Event, this);
		}
	}
}


void ADragon::EndHPSkill()
{
	DeactivateFlameVFX();
}
