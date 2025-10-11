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
	SM_Sword->SetRelativeLocation({-0.258, -78.622, 112.711});
	SM_Sword->SetRelativeRotation(XMFLOAT4{0.507, -0.116, -0.759, 0.392});
	SM_Sword->Rename("Sword");
}

void ASanhwaCharacter::Register()
{
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
		{
			SM_Sword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
		});

	UltimateSceneCameraComp->SetupAttachment(SkeletalMeshComponent, "spine_05");
	UltimateSceneCameraComp->SetRelativeLocation({11.6,167.28,0.0});
	UltimateSceneCameraComp->SetRelativeRotation(XMFLOAT4{-0.073,-0.704,0.078, 0.702});
	if (TestComp_DeleteLater)
	{
		TestComp_DeleteLater->SetupAttachment(SkeletalMeshComponent, "spine_05");
		TestComp_DeleteLater->SetRelativeLocation({11.6,167.28,0.0});
		TestComp_DeleteLater->SetRelativeRotation(XMFLOAT4{-0.073,-0.704,0.078, 0.702});
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
}

void ASanhwaCharacter::CreateIceSpikes(bool bIsUltimate)
{
	XMFLOAT3 ActorLocation = GetActorLocation();
	XMFLOAT3 ActorForwardVector = GetActorForwardVector();


	FTransform SpawnTransform;
	SpawnTransform.Translation = ActorLocation + ActorForwardVector * 100;
	SpawnTransform.Scale3D = {1,1,1};
	SpawnTransform.Rotation = MyMath::VectorToRotationQuaternion(ActorForwardVector);
	std::shared_ptr<AActor> IceSpike;
	if (bIsUltimate)
	{
		IceSpike = GetWorld()->SpawnActor("ASanhwaIceSpikeUltimate", SpawnTransform);
	}
	else
	{
		IceSpike = GetWorld()->SpawnActor("ASanhwaIceSpikeBase", SpawnTransform);	
	}
	std::shared_ptr<ASanhwaIceSpikeBase> S = std::dynamic_pointer_cast<ASanhwaIceSpikeBase>(IceSpike);
	IceSpikes.emplace_back(S); 
}

void ASanhwaCharacter::GetIceSpikes(std::vector<std::shared_ptr<ASanhwaIceSpikeBase>>& IceSpikeVector)
{
	auto RemoveIter = std::remove_if(IceSpikes.begin(),IceSpikes.end(), [](const std::weak_ptr<ASanhwaIceSpikeBase>& IceSpike){ return IceSpike.expired(); });
	IceSpikes.erase(RemoveIter,IceSpikes.end());

	for (std::weak_ptr<ASanhwaIceSpikeBase>& IceSpike : IceSpikes)
	{
		IceSpikeVector.emplace_back(IceSpike.lock());
	}
}
