#include "CoreMinimal.h"
#include "UUltimateBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"

UUltimateBaseComponent::UUltimateBaseComponent()
{
}


void UUltimateBaseComponent::Skill()
{

	MY_LOG("Use Skill - ",EDebugLogLevel::DLL_Warning, "UltimateUltimateUltimateUltimateUltimateUltimate");
}

void UUltimateBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::Q, ETriggerEvent::Started, this, &UUltimateBaseComponent::Skill);
}
