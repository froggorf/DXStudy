// 03.21
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


#pragma once

#include "ImguiPanel.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/RenderCore/EditorScene.h"

enum class EDebugConsoleCommandType
{
	DCCT_Null,
	DCCT_AddConsoleText,
	
};

struct FImguiDebugConsoleCommandData : public FImguiPanelCommandData
{
	FImguiDebugConsoleCommandData() {};
	~FImguiDebugConsoleCommandData() override {};

	DebugText DebugText;
	EDebugConsoleCommandType CommandType;

	// 언리얼엔진의 Struct 비교 방식에서 차용
	// 각 ImguiPanelCommandData 클래스의 ID (고유해야함)
	static UINT GetClassID()
	{
		static CommandIDGenerator Generator;
		return Generator.ID;
	}
	virtual UINT GetTypeID() const {return FImguiPanelCommandData::GetClassID();}
	virtual bool IsOfType(UINT InTypeID) const {return FImguiPanelCommandData::GetClassID() == InTypeID|| FImguiPanelCommandData::IsOfType(InTypeID); }
	
};


class FImguiDebugConsole : public FImguiPanel
{
public:
	FImguiDebugConsole(FScene* SceneData);


	void Draw() override;
	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override;
protected:
private:
public:
protected:
private:
};