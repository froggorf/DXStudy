#pragma once
#include "AMyGameCharacterBase.h"

class UUserWidget;

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void Register() override;
	void CreateWidgetOnBeginPlay() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
private:
	std::shared_ptr<UStaticMeshComponent> SM_Sword;
public:
	static std::string CharacterName;
};
