#include "CoreMinimal.h"
#include "AEnemyBase.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UWidgetComponent.h"
#include "Engine/World/UWorld.h"
#include "MyGame/AnimInstance/Animal/UEnemyAnimInstance.h"
#include "MyGame/BehaviorTree/TestBT.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Component/Health/UHealthComponent.h"
#include "MyGame/Core/AMyGamePlayerController.h"
#include "MyGame/Widget/Health/UEnemyHealthWidget.h"
#include "MyGame/Widget/Health/UHealthWidgetBase.h"

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

	CapsuleComp->SetHalfHeight(50.0f);
	CapsuleComp->SetRadius(50.0f);
	SkeletalMeshComponent->SetRelativeLocation({0,-100,0});
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
	if (AIController)
	{
		AIController->OnPossess(this);
		std::shared_ptr<UTestBT> BT = std::make_shared<UTestBT>();
		BT->Register();
		BT->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Owner", shared_from_this());

		AIController->SetBehaviorTree(BT);

		AIController->GetBehaviorTree()->GetBlackBoard()->SetValue<FBlackBoardValueType_Object>("Player", GetWorld()->GetPlayerController()->GetPlayerCharacter());		
	}

	// Enemy 콜리젼 채널로 변경
	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Enemy);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Enemy);


	HealthComponent->SetMaxHealth(EnemyMaxHealth, true);



}

float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	if (HealthComponent->ApplyDamage(DamageAmount) <=0.0f)
	{
		Death();
	}

	HealthComponent->ApplyDamage(DamageAmount);

	if (AMyGamePlayerController* PC = dynamic_cast<AMyGamePlayerController*>(GetWorld()->GetPlayerController()))
	{
		XMFLOAT4 DefaultColor = {0,0,0,1};
		if (const FMyGameDamageEvent* MyGameDamageEvent = dynamic_cast<const FMyGameDamageEvent*>(&DamageEvent))
		{
			DefaultColor = GetElementColor(MyGameDamageEvent->ElementType);
		}
		PC->SpawnFloatingDamage(GetRootComponent()->GetComponentTransform(), DefaultColor, static_cast<UINT>(DamageAmount));
	}

	if (AIController)
	{
		float NewHealthPercent = HealthComponent->GetHealthPercent() * 100;
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Warning,std::to_string(NewHealthPercent));
		AIController->GetBehaviorTree()->GetBlackBoard()->SetValue<FBlackBoardValueType_FLOAT>("HealthPercent", NewHealthPercent);
	}

	return DamageAmount;
}

void AEnemyBase::PlayAttackMontage(Delegate<> AttackFinishDelegate)
{
	
	if (EnemyAnimInstance)
	{
		MY_LOG("AEnemyBase",EDebugLogLevel::DLL_Warning,"BasicAttack");
		EnemyAnimInstance->DoAttackAnim(AttackFinishDelegate);
	}
	else
	{
		MY_LOG("AEnemyBase",EDebugLogLevel::DLL_Warning,"No Valid EnemyAnimInstance");
	}

}

void AEnemyBase::ApplyBasicAttack()
{
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
	// TODO : Death 애니메이션 재생
	MY_LOG("Death", EDebugLogLevel::DLL_Warning, "Enemy Death");
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);

	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		if (const std::shared_ptr<AActor>& PlayerCharacter = PC->GetPlayerCharacter())
		{
			const std::shared_ptr<FBlackBoard>& BlackBoard = AIController->GetBehaviorTree()->GetBlackBoard();
			float Distance = MyMath::GetDistance(GetActorLocation(), PlayerCharacter->GetActorLocation());
			bool NewValue = Distance > 700.0f;
			BlackBoard->SetValue<FBlackBoardValueType_Bool>("MoveMode", NewValue);		
		}
	}
	
}

AWolf::AWolf()
{
	SkeletalMeshName = "SK_Wolf";
	AnimInstanceName = "UWolfAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100, 30, 100}, 1.0f, 0.0f, 0.0f, true};
	EnemyPower = 75.0f;
}

APig::APig()
{
	SkeletalMeshName = "SK_Pig";
	AnimInstanceName = "UPigAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100,30,100}, 1.0f, 0.0f, 0.0f, true};
}

ADragon::ADragon()
{
	CapsuleComp->SetHalfHeight(250.0f);
	CapsuleComp->SetRadius(250.0f);
	SkeletalMeshComponent->SetRelativeLocation({0,-500,0});
	SkeletalMeshComponent->SetRelativeRotation(XMFLOAT3{0,180,0});

	SkeletalMeshName = "SK_Dragon";
	AnimInstanceName = "UDragonAnimInstance";
	BasicAttackData = FAttackData{XMFLOAT3{100,30,100}, 1.0f, 0.0f, 0.0f, true};
	EnemyPower = 150.0f;
}
