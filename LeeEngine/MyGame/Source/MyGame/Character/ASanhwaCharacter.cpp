#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

ASanhwaCharacter::ASanhwaCharacter()
{
	CharacterMeshName = "SK_UE5MM";
	AnimInstanceName = "USanhwaAnimInstance";

	DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Dodge_Fwd";
	DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Dodge_Bwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE5MM_Roll_Fwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE5MM_Roll_Bwd";
}
