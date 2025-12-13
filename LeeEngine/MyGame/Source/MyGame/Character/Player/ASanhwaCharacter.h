#pragma once
#include "AMyGameCharacterBase.h"

class AIceSpikeBase;
class UUserWidget;

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void Register() override;
	void PossessedBy(AController* NewController) override;
	void UnPossessed() override;
	void BeginPlay() override;

	void OnSkeletalMeshLoadSuccess() override;

	void CreateIceSpikes(bool bIsUltimate);
	
private:
public:
	static std::string CharacterName;

public:
private:
};
