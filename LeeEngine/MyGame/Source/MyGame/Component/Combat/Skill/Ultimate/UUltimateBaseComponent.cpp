#include "CoreMinimal.h"
#include "UUltimateBaseComponent.h"

#include "Engine/Class/Framework/UPlayerInput.h"
#include "Engine/RenderCore/EditorScene.h"

UUltimateBaseComponent::UUltimateBaseComponent()
{
}


bool UUltimateBaseComponent::Skill()
{

	MY_LOG("Use Skill - ",EDebugLogLevel::DLL_Warning, "UltimateUltimateUltimateUltimateUltimateUltimate");
	return false;
}

void UUltimateBaseComponent::BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem)
{
	InputSystem->BindAction(EKeys::Q, ETriggerEvent::Started, this, &UUltimateBaseComponent::Skill);
}
