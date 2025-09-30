#pragma once

#include "CoreMinimal.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Timer/FTimerManager.h"

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

	// 하위클래스에서 원하는 몽타쥬 이름으로 변경 가능
	std::string DodgeMontageName[static_cast<int>(EDodgeDirection::Count)] = {"AM_UE4_Dodge_Fwd", "AM_UE4_Dodge_Bwd"};
	std::string RollMontageName[static_cast<int>(EDodgeDirection::Count)] = {"AM_UE4_Roll_Fwd", "AM_UE4_Roll_Bwd"};

	bool bIsDodging = false;
	bool bIsBackDodge = false;

	FTimerHandle AttackedWhileDodgingHandle;
	FTimerHandle RollingEndHandle;

	// 회피 중 공격 받을 시 AttackedWhileDodgeTriggerTime 이후에 실행될 델리게이트
	float AttackedWhileDodgeTriggerTime = 0.1f;
};
