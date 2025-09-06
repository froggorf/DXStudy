#pragma once
#include "Engine/Components/USceneComponent.h"
#include "Engine/Misc/Delegate.h"

class UPBRTestComponent : public USceneComponent
{
	MY_GENERATE_BODY(UPBRTestComponent)

	Delegate<> OnButtonDelegate;
	Delegate<float> OnRChange;
	Delegate<float> OnGChange;
	Delegate<float> OnBChange;
#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif
};

inline void UPBRTestComponent::DrawDetailPanel(UINT ComponentDepth)
{
	USceneComponent::DrawDetailPanel(ComponentDepth);

	static float R = 1.0f, G = 0.0f, B = 0.0f;
	if (ImGui::SliderFloat("R", &R, 0.0f,1.0f, "%.1f"))
	{
		OnRChange.Broadcast(R);
	}
	if (ImGui::SliderFloat("G", &G, 0.0f,1.0f, "%.1f"))
	{
		OnGChange.Broadcast(G);
	}
	if (ImGui::SliderFloat("B", &B, 0.0f,1.0f, "%.1f"))
	{
		OnBChange.Broadcast(B);
	}

	if (ImGui::Button("PBR"))
	{
		OnButtonDelegate.Broadcast();
	}
}
