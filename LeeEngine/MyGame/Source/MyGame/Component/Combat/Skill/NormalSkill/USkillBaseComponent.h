// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "Engine/Timer/FTimerManager.h"

class UPlayerInput;
class AMyGameCharacterBase;


/*
 Note
 스킬 쿨타임을 Tick에서 줄이는 것보다는 타이머를 사용하는 편이 좋아보여
 CoolDownTimerRepeatTime 마다 쿨타임 감소 로직을 설정
 */

class USkillBaseComponent : public UActorComponent
{
	MY_GENERATE_BODY(USkillBaseComponent)
	USkillBaseComponent();

public:
	// 초기 세팅
	// AMyGameCharacterBase 내에서 Register 에 호출됨
	virtual void Initialize(AMyGameCharacterBase* MyCharacter);
	virtual bool Skill();
	virtual void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem);

	void CoolDown();
protected:
	AMyGameCharacterBase* OwnerCharacter = nullptr;

	float CurrentCoolDownTime = 0.0f;
	// 상속받은 클래스에서 조정
	float SkillCoolDownTime = 10.0f;
	FTimerHandle CoolDownTimerHandle;
	// 0.1f 초마다 타이머 재생 & 쿨타임 감소
	float CoolDownTimerRepeatTime = 0.1f;
};
