// 03.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "ImguiPanel.h"
#include "Engine/RenderCore/EditorScene.h"

class FImguiActorDetail : public FImguiPanel
{
public:
	FImguiActorDetail(FScene* Scene, class FImguiLevelViewport* Owner);

	void Draw() override;

	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override { }

	void InitLevelData();

	// 월드 아웃라이너에서 액터가 선택되었을 때 부모를 통해 호출
	void SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor);

	// Imguizmo 렌더링에 사용
	std::shared_ptr<USceneComponent> GetCurrentSelectedComponent() const;

	std::shared_ptr<UActorComponent> GetCurrentSelectedActorComponent() const;

private:
	// 선택된 월드 아웃라이너 액터의 컴퍼넌트 계층구조를 찾는 함수
	void FindComponentsAndNamesFromActor(const std::shared_ptr<USceneComponent>& TargetComponent, int CurrentHierarchyDepth);

	// Owner 클래스인 ImguiLevelViewport
	class FImguiLevelViewport* LevelViewportOwnerPanel;

	// Detail Panel 변수
	std::shared_ptr<AActor>                       CurrentSelectedActor;
	std::vector<std::shared_ptr<USceneComponent>> SelectActorSceneComponents;
	std::vector<std::string>                      SelectActorSceneComponentNames;
	std::vector<std::shared_ptr<UActorComponent>> SelectActorActorComponents;
	std::vector<std::string>                      SelectActorActorComponentNames;
	int                                           CurrentSelectedComponentIndex = -1;
};
