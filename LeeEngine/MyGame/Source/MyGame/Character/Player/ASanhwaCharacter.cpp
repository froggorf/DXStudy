#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Actor/Sanhwa/ASanhwaIceBase.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/SanHwa/USanhwaSkillComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/Sanhwa/USanhwaUltimateComponent.h"
#include "MyGame/Component/Health/UHealthComponent.h"
#include "MyGame/Core/AMyGamePlayerController.h"
#include "MyGame/Widget/Sanhwa/USanhwaWidget.h"

std::string ASanhwaCharacter::CharacterName  = "Sanhwa";

ASanhwaCharacter::ASanhwaCharacter()
{
	CharacterMeshName = "SK_Sanhwa";
	AnimInstanceName = "USanhwaAnimInstance";
	DeathMontageName = "AM_Sanhwa_Death";

	CombatComponent = std::dynamic_pointer_cast<USanhwaCombatComponent>(CreateDefaultSubobject("SanhwaCombatComp", "USanhwaCombatComponent"));
	SkillComponent = std::dynamic_pointer_cast<USanhwaSkillComponent>(CreateDefaultSubobject("SanhwaSkillComp", "USanhwaSkillComponent"));
	UltimateComponent = std::dynamic_pointer_cast<USanhwaUltimateComponent>(CreateDefaultSubobject("SanhwaUltComp", "USanhwaUltimateComponent"));

	CharacterMaxHealth = 1000.0f;

	ElementType = EElementType::Glacio;
}

void ASanhwaCharacter::Register()
{
	UltimateSceneCameraComp->SetupAttachment(SkeletalMeshComponent, "");
	UltimateSceneCameraComp->SetRelativeLocation({0,100,-200});
	UltimateSceneCameraComp->SetRelativeRotation(XMFLOAT3{0.0,0.0f,0.0f});

	AMyGameCharacterBase::Register();
}

void ASanhwaCharacter::PossessedBy(AController* NewController)
{
	AMyGameCharacterBase::PossessedBy(NewController);

	if (!CharacterWidget)
	{
		CharacterWidget = std::make_shared<USanhwaWidget>();
		CharacterWidget->NativeConstruct();
	}

	if (APlayerController* PC = dynamic_cast<APlayerController*>(NewController))
	{
		PC->AddToViewport(CharacterName, CharacterWidget);	
	}

	if (HealthComponent)
	{
		HealthComponent->RefreshHealthWidget();
	}
}

void ASanhwaCharacter::UnPossessed()
{
	if (APlayerController* PC = dynamic_cast<APlayerController*>(Controller))
	{
		PC->RemoveFromParent(CharacterName);	
	}

	AMyGameCharacterBase::UnPossessed();

}

void ASanhwaCharacter::BeginPlay()
{
	AMyGameCharacterBase::BeginPlay();

	
}

void ASanhwaCharacter::OnSkeletalMeshLoadSuccess()
{
	AMyGameCharacterBase::OnSkeletalMeshLoadSuccess();

	GetRootComponent()->UpdateComponentToWorld();

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
		IceSpikesActor = GetWorld()->SpawnActor("AIceSpikeUltimate", SpawnTransform);
	}
	else
	{
		IceSpikesActor = GetWorld()->SpawnActor("AIceSpikeBase", SpawnTransform);	
	}

	if (const std::shared_ptr<AIceSpikeBase>& IceActor = std::dynamic_pointer_cast<AIceSpikeBase>(IceSpikesActor))
	{
		IceActor->SpawnedBy(this);
	}
}
