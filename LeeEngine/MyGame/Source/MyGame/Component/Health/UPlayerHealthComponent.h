#pragma once
#include "UHealthComponent.h"

class AMyGameCharacterBase;

class UPlayerHealthComponent : public UHealthComponent
{
	MY_GENERATE_BODY(UPlayerHealthComponent)

public:
protected:
	void MarkHealthToWidget() override;
	AMyGameCharacterBase* MyGameCharacter = nullptr;
private:
};
