// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Class/Framework/ACharacter.h"

enum class EDodgeDirection
{
	Forward, Backward, Left, Right, Count
};

class AMyGameCharacterBase : public ACharacter
{
	MY_GENERATE_BODY(AMyGameCharacterBase)
	
	AMyGameCharacterBase();
	
	void BeginPlay() override;

	void BindKeyInputs() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	void Tick(float DeltaSeconds) override;
	//void Backstep();
	void Move(float X, float Y);
	void Look(float X, float Y);
	void MouseRotateStart();
	void MouseRotateEnd();
	void Dodge();
	void DodgeEnd();
	void RollEnd();
	void SetWalk();
	void SetRun();
	void WheelUp();
	void WheelDown();

protected:

	std::shared_ptr<UAnimMontage> AM_Dodge[static_cast<int>(EDodgeDirection::Count)];
	std::shared_ptr<UAnimMontage> AM_Roll[static_cast<int>(EDodgeDirection::Count)];
private:
	bool bRightButtonPressed = false;

	bool bIsBackDodge = false;
	bool bIsDodging = false;

};
