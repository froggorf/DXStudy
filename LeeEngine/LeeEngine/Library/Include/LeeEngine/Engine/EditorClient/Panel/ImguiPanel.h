// 03.21
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석


#pragma once

//#include "EditorClient.h"
#include "Engine/MyEngineUtils.h"

class FScene;

struct FImguiPanelCommandData
{
	FImguiPanelCommandData() = default;
	virtual ~FImguiPanelCommandData() = default;

	enum class EImguiPanelType PanelType;

	// 언리얼엔진의 Struct 비교 방식에서 차용
	// 각 ImguiPanelCommandData 클래스의 ID (고유해야함)
	//static const UINT ClassID = typeid(FImguiPanelCommandData).hash_code();;
	//virtual UINT GetTypeID() const {return FImguiPanelCommandData::ClassID;}
	//virtual bool IsOfType(UINT InTypeID) const {return FImguiPanelCommandData::ClassID == InTypeID; }
};

class FImguiPanel
{
public:
	FImguiPanel(FScene* SceneData);
	virtual ~FImguiPanel(){}

	virtual void Draw() = 0;
	virtual void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) = 0;
protected:
private:
public:
protected:
	FScene* SceneData;
private:
};