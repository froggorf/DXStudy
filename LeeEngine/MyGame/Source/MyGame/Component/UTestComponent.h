// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include <Engine/Animation/UAnimSequence.h>

#include "ThirdParty/ImGui/imgui.h"
#include "Engine/Components/UActorComponent.h"
#include "Engine/Components/UAnimMontage.h"


class UTestComponent : public UActorComponent
{
	MY_GENERATED_BODY(UTestComponent)
public:
	UTestComponent();

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

	void BeginPlay() override;
protected:
private:
public:
	float TestSpeed = 0.0f;
	float TestAngle = 0.0f;

	int TargetAnim = 0;

	float AimOffsetX= 0;
	float AimOffsetY = 0;
	/*ImVec2 DrawSize = ImVec2{400.0f,300.0f};

	XMFLOAT2 CurrentValue = XMFLOAT2{0.0f,0.0f};*/

	std::shared_ptr<UAnimMontage> AM_Test;
protected:
private:
};
