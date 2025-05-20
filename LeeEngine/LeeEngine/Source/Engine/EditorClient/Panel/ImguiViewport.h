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

class FImguiActorDetail;
class FImguiWorldOutliner;

enum class ELevelViewportCommandType
{
	LVCT_Null,
	LVCT_AddActorToWorldOutliner,
	LVCT_SetViewportSizeToEditorViewportSize,
	LVCT_GetEditorViewMatrices,
	LVCT_SelectActorFromWorldOutliner,
	LVCT_ChangeLevelInitialize
};

struct FImguiLevelViewportCommandData : FImguiPanelCommandData
{
	FImguiLevelViewportCommandData()
	{
	};

	~FImguiLevelViewportCommandData() override
	{
	};

	ELevelViewportCommandType CommandType;
	std::shared_ptr<AActor>   NewPendingAddActor;
	std::shared_ptr<AActor>   NewSelectedActorFromWorldOutliner;
	FViewMatrices*            ViewMatrices;

	// 언리얼엔진의 Struct 비교 방식에서 차용
	// 각 ImguiPanelCommandData 클래스의 ID (고유해야함)
	static UINT GetClassID()
	{
		static CommandIDGenerator Generator;
		return Generator.ID;
	}

	UINT GetTypeID() const override
	{
		return GetClassID();
	}

	bool IsOfType(UINT InTypeID) const override
	{
		return GetClassID() == InTypeID || FImguiPanelCommandData::IsOfType(InTypeID);
	}
};

class FImguiLevelViewport : public FImguiPanel
{
public:
	FImguiLevelViewport(FScene* SceneData);

	void Draw() override;
	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override;

	// 월드 아웃라이너 패널에서 액터를 선택했을 시 호출
	void SelectActorFromWorldOutliner(const std::shared_ptr<AActor>& NewSelectedActor);

private:

	// CurrentSelectedActor의 Imguizmo 기즈모 렌더링 + 이동 조작 Imguizmo 커맨드 큐 추가
	void DrawImguizmoSelectedActor(float AspectRatio);

	// Level Viewport 내에서 우클릭 + WASD 및 마우스 인풋 시
	// 에디터 카메라 이동을 적용하는 함수
	void EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta);

	FImguiWorldOutliner* GetWorldOutlinerPanel() const
	{
		return WorldOutlinerPanel.get();
	}

	FImguiActorDetail* GetActorDetailPanel() const
	{
		return ActorDetailPanel.get();
	}
public:
	static ImVec2 PreviousViewPortSize;
private:
	std::unique_ptr<FImguiWorldOutliner> WorldOutlinerPanel;
	std::unique_ptr<FImguiActorDetail>   ActorDetailPanel;

	// Level Viewport 변수
	bool   bResizeEditorRenderTargetAtEndFrame = false;
	ImVec2 ResizeEditorRenderTargetSize        = {};

	FViewMatrices EditorViewMatrices;
};
