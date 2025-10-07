// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Timer/FTimerManager.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"

class USanhwaSkillComponent : public USkillBaseComponent
{
	MY_GENERATE_BODY(USanhwaSkillComponent)
	USanhwaSkillComponent();

public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;

protected:
};
