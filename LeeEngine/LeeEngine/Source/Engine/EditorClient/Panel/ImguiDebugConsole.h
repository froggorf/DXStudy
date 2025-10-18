// 03.21
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "ImguiPanel.h"
#include "Engine/RenderCore/EditorScene.h"

enum class EDebugConsoleCommandType
{
	DCCT_Null,
	DCCT_AddConsoleText,
};

struct FImguiDebugConsoleData
{
	// 검색에 사용되는 string
	static std::string DebugConsoleSearchText;

	// 디버그 텍스트 벡터
	static std::vector<DebugText> DebugConsoleText;
	// Pending Add
	static std::vector<DebugText> PendingAddDebugConsoleText;
	// 검색 시 string에 맞는 디버그 문구만 모은 벡터
	static std::vector<DebugText> SearchingDebugConsoleText;
};

struct FImguiDebugConsoleCommandData : FImguiPanelCommandData
{
	FImguiDebugConsoleCommandData()= default;

	~FImguiDebugConsoleCommandData() override= default;

	DebugText                DebugText;
	EDebugConsoleCommandType CommandType;

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

class FImguiDebugConsole : public FImguiPanel
{
public:
	FImguiDebugConsole(FScene* SceneData);

	void Draw() override;
	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override;

private:
	void SearchDebugConsole();

	

	int IsInit = 0;
};
