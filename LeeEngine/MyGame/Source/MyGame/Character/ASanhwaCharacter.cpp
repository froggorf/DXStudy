#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"

ASanhwaCharacter::ASanhwaCharacter()
{
	CharacterMeshName = "SK_UE5MM";
	AnimInstanceName = "USanhwaAnimInstance";

	DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Dodge_Fwd";
	DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";

	CombatComponent = std::make_shared<USanhwaCombatComponent>();
	// TODO : 산화의 타입으로 변경해주기
	SkillComponent = std::make_shared<USkillBaseComponent>();
	UltimateComponent = std::make_shared<UUltimateBaseComponent>();
}

