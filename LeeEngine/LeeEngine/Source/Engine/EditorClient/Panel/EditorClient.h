// 03.21
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include <any>
#include <functional>

#include "ImguiPanel.h"
#include "Engine/MyEngineUtils.h"

enum class EImguiPanelType
{
	IPT_NULL,
	IPT_MainPanel,     // 메인 패널
	IPT_LevelViewport, // FImguiViewport (+ FImguiWorldOutliner + FImguiActorDetail
	IPT_DebugConsole,  // FImguiDebugConsole

	IPT_Count,
};

class FEditorClient
{
public:
	FEditorClient(class FScene* SceneData);

public:
	void AddPanelCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData);

	void Draw();

protected:
	std::vector<std::unique_ptr<FImguiPanel>> ImguiPanels;
};
