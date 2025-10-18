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

	void Look(float X, float Y) override;

	void ToAimMode();
	void ToNormalMode();
	bool IsAimMode() const {return bIsAimMode;}

	const std::shared_ptr<USpringArmComponent>& GetAimModeSpringArm() const { return AimModeSpringArm; }
	const std::shared_ptr<UCameraComponent>& GetAimModeCameraComp() const { return AimModeCameraComp; }

private:
public:
	static std::string CharacterName;

public:
private:
	std::shared_ptr<USpringArmComponent> AimModeSpringArm;
	std::shared_ptr<UCameraComponent> AimModeCameraComp;
	bool bIsAimMode = false;

	float AimModePitchMin = -70.0f;
	float AimModePitchMax = 70.0f;
};
