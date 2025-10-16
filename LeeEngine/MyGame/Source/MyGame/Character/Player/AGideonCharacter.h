#pragma once
#include "AMyGameCharacterBase.h"

class UUserWidget;

class AGideonCharacter : public AMyGameCharacterBase
{
	MY_GENERATE_BODY(AGideonCharacter)

	AGideonCharacter();
	void Register() override;

	void SpawnFireBall(const FTransform& SpawnTransform, const FAttackData& AttackData, const XMFLOAT3& TargetPosition);
private:
public:
	static std::string CharacterName;

public:
private:
};
