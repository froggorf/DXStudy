// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Physics/UPhysicsEngine.h"
#include "MyGame/Component/Combat/Skill/Ultimate/UUltimateBaseComponent.h"

class UGideonUltimateComponent : public UUltimateBaseComponent
{
	MY_GENERATE_BODY(UGideonUltimateComponent)
	UGideonUltimateComponent() = default;


public:
	bool TrySkill() override;
	void Initialize(AMyGameCharacterBase* MyCharacter) override;
	void ApplyUltimateAttack() override;
};
