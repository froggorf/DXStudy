// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"

class UGideonSkillComponent : public USkillBaseComponent
{
	MY_GENERATE_BODY(UGideonSkillComponent)
	UGideonSkillComponent() = default;

public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

	void ApplySkillAttack() override;
protected:
};
