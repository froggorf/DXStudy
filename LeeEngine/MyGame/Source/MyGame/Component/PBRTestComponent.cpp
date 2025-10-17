#include "CoreMinimal.h"
#include "PBRTestComponent.h"

UPBRTestComponent* UPBRTestComponent::Instance = nullptr;

UPBRTestComponent::UPBRTestComponent()
{
	Rename("Monochrome Test Comp");
}

void UPBRTestComponent::TurnOnMono()
{


}


void UPBRTestComponent::TurnOffMono()
{
}

void UPBRTestComponent::ObjectSelect(int index)
{

}

#ifdef WITH_EDITOR
void UPBRTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	USceneComponent::DrawDetailPanel(ComponentDepth);
	static bool Mono = false;
	if (ImGui::Checkbox("Monochrome", &Mono))
	{
		if (Mono)
		{
			TurnOnMono();
		}
		else
		{
			TurnOffMono();
		}
	}

	static float Distance = 0.0f;
	if (ImGui::SliderFloat("Monochrome Distance", &Distance, 0.0f, 2000, "%.2f"))
	{
	}

	ImGui::Text("TargetObject");
	if (ImGui::Button("Chair"))
	{
		ObjectSelect(0);
	}
	if (ImGui::Button("Couch"))
	{
		ObjectSelect(1);
	}
	if (ImGui::Button("Character"))
	{
		ObjectSelect(2);
	}
}
#endif