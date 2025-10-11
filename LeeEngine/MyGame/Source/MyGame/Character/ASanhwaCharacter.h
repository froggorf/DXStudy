#pragma once
#include "AMyGameCharacterBase.h"

class ASanhwaIceSpikeBase;
class UUserWidget;

class ASanhwaCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(ASanhwaCharacter)

	ASanhwaCharacter();
	void Register() override;
	void CreateWidgetOnBeginPlay() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void CreateIceSpikes(bool bIsUltimate);
private:
	std::shared_ptr<UStaticMeshComponent> SM_Sword;
public:
	static std::string CharacterName;

public:
	std::vector<std::weak_ptr<ASanhwaIceSpikeBase>>& GetIceSpikes() { return IceSpikes;}
private:
	std::vector<std::weak_ptr<ASanhwaIceSpikeBase>> IceSpikes;
};
