#pragma once
#include "AMyGameCharacterBase.h"

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void CreateWidgetOnBeginPlay() override;
};