#pragma once
#include "AMyGameCharacterBase.h"

class UUserWidget;

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void CreateWidgetOnBeginPlay() override;

private:
public:
	static std::string CharacterName;
};
