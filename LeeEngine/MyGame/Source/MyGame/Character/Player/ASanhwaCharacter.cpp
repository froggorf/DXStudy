#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Actor/Sanhwa/ASanhwaIceBase.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/SanHwa/USanhwaSkillComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/Sanhwa/USanhwaUltimateComponent.h"
#include "MyGame/Core/AMyGamePlayerController.h"
#include "MyGame/Widget/Sanhwa/USanhwaWidget.h"

std::string ASanhwaCharacter::CharacterName  = "Sanhwa";

ASanhwaCharacter::ASanhwaCharacter()
{
	CharacterMeshName = "SK_UE5MM";
	AnimInstanceName = "USanhwaAnimInstance";

	DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Dodge_Fwd";
	DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";

	CombatComponent = std::dynamic_pointer_cast<USanhwaCombatComponent>(CreateDefaultSubobject("SanhwaCombatComp", "USanhwaCombatComponent"));
	SkillComponent = std::dynamic_pointer_cast<USanhwaSkillComponent>(CreateDefaultSubobject("SanhwaSkillComp", "USanhwaSkillComponent"));
	UltimateComponent = std::dynamic_pointer_cast<USanhwaUltimateComponent>(CreateDefaultSubobject("SanhwaUltComp", "USanhwaUltimateComponent"));

	SM_Sword = std::make_shared<UStaticMeshComponent>();
	SM_Sword->SetupAttachment(SkeletalMeshComponent, "hand_r");
	
	SM_Sword->SetRelativeScale3D({0.15f,0.15f,0.15f});
	SM_Sword->SetRelativeLocation({-0.258f, -78.622f, 112.711f});
	SM_Sword->SetRelativeRotation(XMFLOAT4{0.507f, -0.116f, -0.759f, 0.392f});
	SM_Sword->Rename("Sword");

	CharacterMaxHealth = 20000.0f;

	ElementType = EElementType::Glacio;
}

void ASanhwaCharacter::Register()
{
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
		{
			SM_Sword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
		});

	UltimateSceneCameraComp->SetupAttachment(SkeletalMeshComponent, "spine_05");
	UltimateSceneCameraComp->SetRelativeLocation({11.6f,167.28f,0.0f});
	UltimateSceneCameraComp->SetRelativeRotation(XMFLOAT4{-0.073f,-0.704f,0.078f, 0.702f});
	if (TestComp_CheckCameraPos)
	{
		TestComp_CheckCameraPos->SetupAttachment(SkeletalMeshComponent, "spine_05");
		TestComp_CheckCameraPos->SetRelativeLocation({11.6f,167.28f,0.0f});
		TestComp_CheckCameraPos->SetRelativeRotation(XMFLOAT4{-0.073f,-0.704f,0.078f, 0.702f});
	}
	
	
	//UltimateSceneSpringArm->SetRelativeRotation(XMFLOAT4{-0.04, 0.93, 0.106, 0.35});

	AMyGameCharacterBase::Register();

	
}

void ASanhwaCharacter::CreateWidgetOnBeginPlay()
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

	SM_Sword->GetBodyInstance()->SetSimulatePhysics(false);
	SM_Sword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SM_Sword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SM_Sword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);

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
}
