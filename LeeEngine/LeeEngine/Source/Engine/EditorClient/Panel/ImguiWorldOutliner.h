// 03.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "ImguiPanel.h"
#include "Engine/RenderCore/EditorScene.h"

class FImguiWorldOutliner : public FImguiPanel
{
public:
	FImguiWorldOutliner(FScene* Scene, class FImguiLevelViewport* Owner);

	void Draw() override;

	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override { } 
	void InitLevelData();
	void PendingAddWorldOutlinerActor(const std::shared_ptr<AActor>& NewActor);

private:
	// Owner 클래스인 ImguiLevelViewport
	class FImguiLevelViewport* LevelViewportOwnerPanel;

	// 월드 아웃라이너
	std::vector<std::shared_ptr<AActor>> WorldOutlinerActors;
	std::vector<std::shared_ptr<AActor>> PendingAddWorldOutlinerActors;
};
