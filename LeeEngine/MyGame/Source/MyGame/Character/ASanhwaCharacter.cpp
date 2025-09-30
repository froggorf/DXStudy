#include "CoreMinimal.h"
#include "ASanhwaCharacter.h"

ASanhwaCharacter::ASanhwaCharacter()
{
	DodgeMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE4_Dodge_Fwd";
	DodgeMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE4_Dodge_Bwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Forward)] = "AM_UE4_Roll_Fwd";
	RollMontageName[static_cast<int>(EDodgeDirection::Backward)] = "AM_UE4_Roll_Bwd";
}
