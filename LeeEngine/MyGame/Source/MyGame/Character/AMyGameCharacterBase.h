// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Class/Framework/ACharacter.h"

enum class EDodgeDirection
{
	Forward, Backward, Count
};

class IDodgeInterface
{
public:
	void LoadAnimMontages();

	virtual void Dodge() = 0;
	virtual void DodgeEnd();
	virtual void ChangeToRoll() = 0;
	virtual void RollEnd();
	virtual void AttackedWhileDodge() = 0;

	void AddMonochromePostprocess();
	void RemoveMonochromePostprocess();
protected:
	std::shared_ptr<UAnimMontage> AM_Dodge[static_cast<int>(EDodgeDirection::Count)];
	std::shared_ptr<UAnimMontage> AM_Roll[static_cast<int>(EDodgeDirection::Count)];

	std::string DodgeMontageName[static_cast<int>(EDodgeDirection::Count)] = {"AM_UE4_Dodge_Fwd", "AM_UE4_Dodge_Bwd"};
	std::string RollMontageName[static_cast<int>(EDodgeDirection::Count)] = {"AM_UE4_Roll_Fwd", "AM_UE4_Roll_Bwd"};

	bool bIsDodging = false;
	bool bIsBackDodge = false;

	FTimerHandle AttackedWhileDodgingHandle;
	FTimerHandle RollingEndHandle;

	// 회피 중 공격 받을 시 AttackedWhileDodgeTriggerTime 이후에 실행될 델리게이트
	float AttackedWhileDodgeTriggerTime = 0.1f;
};

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

	
protected:



	void Dodge() override;
	void DodgeEnd() override;
	void ChangeToRoll() override;
	void RollEnd() override;
	void AttackedWhileDodge() override;
private:
	bool bRightButtonPressed = false;



	
	
};
