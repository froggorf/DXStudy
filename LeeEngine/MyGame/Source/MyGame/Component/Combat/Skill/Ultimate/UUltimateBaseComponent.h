// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"


class UCameraComponent;
class UPlayerInput;

class UUltimateBaseComponent : public USkillBaseComponent
{
	MY_GENERATE_BODY(UUltimateBaseComponent)
	UUltimateBaseComponent();

public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;
	void SetMaxUltimateGauge(const float NewGauge) { MaxUltimateGauge = NewGauge; }

	bool TrySkill() override;
	void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem) override;

	void CoolDown() override;
	bool CanUseSkill() override;

	void AddUltimateGauge(const float NewAddGauge);
private:
	float CurrentUltimateGauge = 0.0f;
	float MaxUltimateGauge = 0.0f;

};
