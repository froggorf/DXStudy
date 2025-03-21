// 03.21
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


#pragma once

#include "ImguiPanel.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/SceneView.h"
#include "Engine/RenderCore/EditorScene.h"


/*
 * 레벨의 에디터 뷰포트와
 * WorldOutliner, ActorDetail을
 * 한번에 관리하고 렌더링 하는 클래스
 */

enum class ELevelViewportCommandType
{
	LVCT_Null,
	LVCT_AddActorToWorldOutliner,
	LVCT_SetViewportSizeToEditorViewportSize,
	LVCT_GetEditorViewMatrices,
	LVCT_ClearCurrentLevelData,
};

struct FImguiLevelViewportCommandData: public FImguiPanelCommandData
{
	FImguiLevelViewportCommandData() {};
	~FImguiLevelViewportCommandData() override {};

	ELevelViewportCommandType CommandType;
	std::shared_ptr<AActor> NewPendingAddActor;
	FViewMatrices* ViewMatrices;

	// 언리얼엔진의 Struct 비교 방식에서 차용
	// 각 ImguiPanelCommandData 클래스의 ID (고유해야함)
	static UINT GetClassID()
	{
		static CommandIDGenerator Generator;
		return Generator.ID;
	}
	virtual UINT GetTypeID() const {return GetClassID();}
	virtual bool IsOfType(UINT InTypeID) const {return GetClassID() == InTypeID|| FImguiPanelCommandData::IsOfType(InTypeID); }

};


class FImguiLevelViewport : public FImguiPanel
{
public:
	FImguiLevelViewport(FScene* SceneData);

	void Draw() override;
	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override;
protected:
private:
	// 레벨 변경 시 현재의 레벨 데이터를 초기화 하는 함수
	void InitLevelData();

	// 월드 아웃라이너 패널에서 액터를 선택했을 시 호출
	void SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor);

	// 선택된 월드 아웃라이너 액터의 컴퍼넌트 계층구조를 찾는 함수
	void FindComponentsAndNamesFromActor(const std::shared_ptr<USceneComponent>& TargetComponent, int CurrentHierarchyDepth);

	// CurrentSelectedActor의 Imguizmo 기즈모 렌더링 + 이동 조작 Imguizmo 커맨드 큐 추가
	void DrawImguizmoSelectedActor(float AspectRatio);

	// Level Viewport 내에서 우클릭 + WASD 및 마우스 인풋 시
	// 에디터 카메라 이동을 적용하는 함수
	void EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta);
public:
protected:
private:
	// 월드 아웃라이너
	std::vector<std::shared_ptr<AActor>> WorldOutlinerActors;
	std::vector<std::shared_ptr<AActor>> PendingAddWorldOutlinerActors;
	std::shared_ptr<AActor> CurrentSelectedActor;
	std::vector<std::shared_ptr<USceneComponent>> SelectActorComponents;
	std::vector<std::string> SelectActorComponentNames;


	// Level Viewport 변수
	bool bResizeEditorRenderTargetAtEndFrame=false;
	ImVec2 ResizeEditorRenderTargetSize = {};

	int CurrentSelectedComponentIndex = -1;

	FViewMatrices EditorViewMatrices;

};