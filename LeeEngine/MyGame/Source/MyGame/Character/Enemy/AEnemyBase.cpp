#include "CoreMinimal.h"
#include "AEnemyBase.h"

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UWidgetComponent.h"
#include "Engine/World/UWorld.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Component/Health/UHealthComponent.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"
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
		CharacterMovement->MaxWalkSpeed = 600;
		CharacterMovement->Braking = 4096;
	}

	SkeletalMeshComponent->SetRelativeLocation({0,-85,0});
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

	AssetManager::GetAsyncAssetCache("SM_DeferredSphere",[this](std::shared_ptr<UObject> Object)
		{
			TempStaticMesh->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			TempStaticMesh->SetCollisionObjectType(ECollisionChannel::Enemy);
			TempStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		});

	QueryCheckCapsuleComp->SetCollisionObjectType(ECollisionChannel::Enemy);
	QueryCheckCapsuleComp->GetBodyInstance()->SetObjectType(ECollisionChannel::Enemy);

	std::shared_ptr<UHealthWidgetBase> EnemyHealthWidget = std::make_shared<UEnemyHealthWidget>();
	HealthWidgetComp->SetWidget(EnemyHealthWidget);
	HealthComponent->SetHealthWidget(EnemyHealthWidget);

	//AssetManager::GetAsyncAssetCache(CharacterMeshName,[this](std::shared_ptr<UObject> Object)
	//	{
	//		SkeletalMeshComponent->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
	//	});
	//SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceName);
}

void AEnemyBase::BeginPlay()
{
	ACharacter::BeginPlay();

	AIController = std::dynamic_pointer_cast<AAIController>(GetWorld()->SpawnActor("AAIController", FTransform{}));
	if (AIController)
	{

		AIController->SetBehaviorTree()
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

	return DamageAmount;
}

void AEnemyBase::Death()
{
	// TODO : Death 애니메이션 재생
	MY_LOG("Death", EDebugLogLevel::DLL_Warning, "Character Death");
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);
}
