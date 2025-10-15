#pragma once
#include "AMyGameCharacterBase.h"

class UUserWidget;

class AGideonCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(AGideonCharacter)

	AGideonCharacter();
	void Register() override;
private:
public:
	static std::string CharacterName;

public:
private:
};
