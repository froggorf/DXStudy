// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"
#include "MyGame/Interface/Dodge/IDodgeInterface.h"

class AMyGameCharacterBase : public ACharacter, public IDodgeInterface
{
	MY_GENERATE_BODY(AMyGameCharacterBase)
	
	AMyGameCharacterBase();
	void Register() override;
	void BeginPlay() override;

	void BindKeyInputs() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	void Tick(float DeltaSeconds) override;
	void Move(float X, float Y);
	void Look(float X, float Y);
	void MouseRotateStart();
	void MouseRotateEnd();
	void SetWalk();
	void SetRun();
	void WheelUp();
	void WheelDown();

private:
	bool bRightButtonPressed = false;

	// IDodgeInterface
protected:
	void Dodge() override;
	void DodgeEnd() override;
	void ChangeToRoll() override;
	void RollEnd() override;
	void AttackedWhileDodge() override;




	
	
};
