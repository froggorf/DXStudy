#include "UTestComponent.h"

#include <Engine/RenderCore/EditorScene.h>

UTestComponent::UTestComponent()
{
}

void UTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	UActorComponent::DrawDetailPanel(ComponentDepth);

	if(ImGui::SliderFloat("TestValue", &TestValue1, 0.0f,600.0f))
	{
		
	}
}
