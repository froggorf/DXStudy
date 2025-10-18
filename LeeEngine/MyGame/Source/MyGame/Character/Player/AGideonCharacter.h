#pragma once
#include "AMyGameCharacterBase.h"

class UUserWidget;

class AGideonCharacter final : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(AGideonCharacter)

	AGideonCharacter();
	void Register() override;

	void BindKeyInputs() override;
	void SpawnFireBall(const FTransform& SpawnTransform, const FAttackData& AttackData, const XMFLOAT3& TargetPosition);

	void CreateWidgetOnBeginPlay() override;

	void ToAimMode();
	void ToNormalMode();
	bool IsAimMode() const {return bIsAimMode;}
private:
public:
	static std::string CharacterName;

public:
private:
	std::shared_ptr<UCameraComponent> AimModeCameraComp;
	bool bIsAimMode = false;
};
