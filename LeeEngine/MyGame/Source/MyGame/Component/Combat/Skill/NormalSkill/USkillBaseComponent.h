// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "Engine/Timer/FTimerManager.h"

class UAnimMontage;
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

	// 0번째 스킬이 사용 가능한지 체크
	virtual bool CanUseSkill();
	// 스킬 도전
	virtual void TrySkill();
	// 스킬 콤보가 존재할 경우, 애니메이션 Blend Out ~ +CheckDoNextSkillTime 시간동안 버튼을 다시 누르는지 체크하는 함수
	virtual void CheckDoNextSkill();
	// CanUseSkill() 일 경우 스킬을 실제로 시전하는 함수
	virtual void DoSkill(UINT SkillIndex);
	// 스킬 몽타쥬 BlendOut 시 Broadcast 되는 함수
	virtual void OnSkillBlendOut();

	// 키 입력
	virtual void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem);

	// 쿨타임 감소 (타이머로 연동)
	void CoolDown();

	float GetSkillMoveDistance(size_t SkillIndex) const {if (SkillIndex >= SkillMoveDistance.size()) return 0.0f; return SkillMoveDistance[SkillIndex];}
	float GetSkillAnimLength(size_t SkillIndex) const;
protected:
	// NOTE: Initialize 안에서 사용하여 하위클래스에서 값을 조정
	void SetSkillMontagesAndCoolDown(const std::vector<std::string>& NewMontageNames, const float NewSkillCooldown);
	void SetSkillMoveDistance(const std::vector<float>& NewMoveDistance);
	void SetSkillDelegates(const std::vector<Delegate<>>& NewStartDelegates, const std::vector<Delegate<>>& NewBlendOutDelegates);
protected:
	AMyGameCharacterBase* OwnerCharacter = nullptr;

	// ================================ Cool Down ================================
	// 기본 쿨타임 시간 (입력값 없을시 default 사용)
	static constexpr float DefaultSkillCoolDownTime = 10.0f;
	float SkillCoolDownTime;
	float CurrentCoolDownTime;
	bool bIsCurrentPlayingSkill = false;
	FTimerHandle CoolDownTimerHandle;
	// 0.1f 초마다 타이머 재생 & 쿨타임 감소
	static constexpr float CoolDownTimerRepeatTime = 0.1f;

	// 스킬 콤보가 더 있을 경우, 애니메이션 종료 후 해당 시간 안에 입력 시 다음 콤보 스킬이 발동할 수 있도록 하는 시간
	static constexpr float CheckDoNextSkillTime = 1.0f;
	float CurrentCheckDoNextSkillTime = 0.0f;

	// 스킬 콤보를 이어 사용할지 계산하는 함수
	FTimerHandle CheckDoNextSkillTimerHandle;

	float LastPressKeyTime = 0.0f;

	// ================================ Skill ================================
	std::vector<std::shared_ptr<UAnimMontage>> SkillAnimMontages;
	std::vector<float> SkillMoveDistance;
	std::vector<Delegate<>>	SkillStartDelegates;
	std::vector<Delegate<>> SkillBlendOutDelegates;

	UINT CurrentSkillCombo = 0;
};
