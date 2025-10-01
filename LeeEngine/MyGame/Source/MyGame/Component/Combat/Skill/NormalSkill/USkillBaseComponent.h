// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"

class UPlayerInput;
class AMyGameCharacterBase;

class USkillBaseComponent : public UActorComponent
{
	MY_GENERATE_BODY(USkillBaseComponent)
	USkillBaseComponent();

public:
	// AMyGameCharacterBase 내에서 Register 에 호출됨
	// 초기 세팅
	virtual void Initialize(AMyGameCharacterBase* MyCharacter);
	virtual void Skill();
	virtual void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem);

protected:
	AMyGameCharacterBase* OwnerCharacter = nullptr;
};
