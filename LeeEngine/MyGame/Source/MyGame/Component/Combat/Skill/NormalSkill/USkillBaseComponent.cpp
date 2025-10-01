#include "CoreMinimal.h"
#include "USkillBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"

USkillBaseComponent::USkillBaseComponent()
{
}

void USkillBaseComponent::Initialize(AMyGameCharacterBase* MyCharacter)
{
	OwnerCharacter = MyCharacter;
}

void USkillBaseComponent::Skill()
{
	MY_LOG("Use Skill",EDebugLogLevel::DLL_Warning, "Skill Use");
}

void USkillBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::E, ETriggerEvent::Started, this, &USkillBaseComponent::Skill);
}
