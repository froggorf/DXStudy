#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

#include "Engine/World/UWorld.h"
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
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
		{
			SM_Sword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
		});
	SM_Sword->SetRelativeScale3D({0.15f,0.15f,0.15f});
	SM_Sword->SetRelativeLocation({-0.258, -78.622, 112.711});
	SM_Sword->SetRelativeRotation(XMFLOAT4{0.507, -0.116, -0.759, 0.392});
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
	SM_Sword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SM_Sword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SM_Sword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);
}

