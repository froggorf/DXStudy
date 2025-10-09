// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "MyGame/Component/Combat/Skill/Ultimate/UUltimateBaseComponent.h"


class UPlayerInput;

class USanhwaUltimateComponent : public UUltimateBaseComponent
{
	MY_GENERATE_BODY(USanhwaUltimateComponent)
	USanhwaUltimateComponent() = default;


public:
	void Initialize(AMyGameCharacterBase* MyCharacter) override;
};
