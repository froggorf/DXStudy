// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"


class UPlayerInput;

class UUltimateBaseComponent : public USkillBaseComponent
{
	MY_GENERATE_BODY(UUltimateBaseComponent)
	UUltimateBaseComponent();

public:
	// AMyGameCharacterBase 내에서 Register 에 호출됨
	// 초기 세팅
	bool Skill() override;
	void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem) override;

};
