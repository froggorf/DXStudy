#pragma once
#include "Engine/Class/Framework/APlayerController.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Misc/Delegate.h"
#include "Engine/RenderCore/RenderingThread.h"
#include "Engine/World/UWorld.h"

class UPBRTestComponent : public USceneComponent
{
	MY_GENERATE_BODY(UPBRTestComponent)

	UPBRTestComponent();
	void TurnOnMono();
	void TurnOffMono();

	void ObjectSelect(int index);

	std::shared_ptr<USceneComponent> TargetComp[3];

	static UPBRTestComponent* Instance;
#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif
};


