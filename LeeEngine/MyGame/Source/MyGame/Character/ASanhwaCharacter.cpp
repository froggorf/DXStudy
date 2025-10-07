#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/SanHwa/USanhwaSkillComponent.h"
#include "MyGame/Widget/Sanhwa/USanhwaWidget.h"

ASanhwaCharacter::ASanhwaCharacter()
{
	CharacterMeshName = "SK_UE5MM";
	AnimInstanceName = "USanhwaAnimInstance";

	DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Dodge_Fwd";
	DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";

	CombatComponent = std::dynamic_pointer_cast<USanhwaCombatComponent>(CreateDefaultSubobject("SanhwaCombatComp", "USanhwaCombatComponent"));
	// TODO : 산화의 타입으로 변경해주기
	SkillComponent = std::make_shared<USanhwaSkillComponent>();
	UltimateComponent = std::make_shared<UUltimateBaseComponent>();
}

void ASanhwaCharacter::CreateWidgetOnBeginPlay()
{
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		std::shared_ptr<USanhwaWidget> MyTestWidget = std::make_shared<USanhwaWidget>();
		PC->CreateWidget("DefaultWidget", MyTestWidget);
	}
}

