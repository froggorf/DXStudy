#include "CoreMinimal.h"
#include "AGideonCharacter.h"

#include "MyGame/Actor/Sanhwa/ASanhwaIceBase.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/SanHwa/USanhwaSkillComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/Sanhwa/USanhwaUltimateComponent.h"
#include "MyGame/Core/AMyGamePlayerController.h"

std::string AGideonCharacter::CharacterName  = "Gideon";

AGideonCharacter::AGideonCharacter()
{
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = true;
		CharacterMovement->Acceleration = 1024.0f;
		CharacterMovement->RotationRate = XMFLOAT3{0.0f, 1500.0f, 0.0f};
		CharacterMovement->MaxWalkSpeed = 500;
		CharacterMovement->Braking = 2048;
	}

	CharacterMeshName = "SK_Gideon";
	AnimInstanceName = "UGideonAnimInstance";

	//DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Dodge_Fwd";
	//DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	//RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	//RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";

	//CombatComponent = std::dynamic_pointer_cast<USanhwaCombatComponent>(CreateDefaultSubobject("SanhwaCombatComp", "USanhwaCombatComponent"));
	//SkillComponent = std::dynamic_pointer_cast<USanhwaSkillComponent>(CreateDefaultSubobject("SanhwaSkillComp", "USanhwaSkillComponent"));
	//UltimateComponent = std::dynamic_pointer_cast<USanhwaUltimateComponent>(CreateDefaultSubobject("SanhwaUltComp", "USanhwaUltimateComponent"));

	CharacterMaxHealth = 20000.0f;

	ElementType = EElementType::Havoc;
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

	
}/*

void AGideonCharacter::CreateWidgetOnBeginPlay()
{
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		std::shared_ptr<USanhwaWidget> SanhwaWidget = std::make_shared<USanhwaWidget>();
		PC->CreateWidget(CharacterName, SanhwaWidget);
		CharacterWidget = SanhwaWidget;
	}
}

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
