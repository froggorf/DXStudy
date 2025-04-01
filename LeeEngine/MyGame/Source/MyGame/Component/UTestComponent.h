// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include <Engine/Animation/UAnimSequence.h>

#include "ThirdParty/ImGui/imgui.h"
#include "Engine/Components/UActorComponent.h"


class UTestComponent : public UActorComponent
{
	MY_GENERATED_BODY(UTestComponent)
public:
	UTestComponent();

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

	//void GetAnimsForBlend(XMFLOAT2& OutCurrentValue, std::vector<std::shared_ptr<FAnimClipPoint>>& OutPoints);

	//const std::vector<std::shared_ptr<FAnimClipEdge>>& GetCurrentEdge() const {return CurrentEdge;}
	//const std::vector<std::shared_ptr<FAnimClipTriangle>>& GetCurrentTriangles() const {return CurrentTriangles;}
	//const std::vector<std::shared_ptr<FAnimClipPoint>>& GetTestValue() const {return CurrentPoints;}
protected:
private:
public:
	float TestValue1 = 0.0f;

	ImVec2 DrawSize = ImVec2{400.0f,300.0f};

	XMFLOAT2 CurrentValue = XMFLOAT2{0.0f,0.0f};


protected:
private:
};