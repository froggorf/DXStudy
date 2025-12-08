#pragma once
#include "AMyGameCharacterBase.h"

class AIceSpikeBase;
class UUserWidget;

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void Register() override;
	void CreateWidgetOnBeginPlay() override;
	void BeginPlay() override;
	
	void CreateIceSpikes(bool bIsUltimate);
	
private:
	std::shared_ptr<UStaticMeshComponent> SM_Sword;
public:
	static std::string CharacterName;

public:
private:
};
