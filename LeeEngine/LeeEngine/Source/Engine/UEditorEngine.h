// 02.22
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#ifdef WITH_EDITOR
#include "UEngine.h"
#include "RenderCore/RenderingThread.h"

#include <atlImage.h>

enum class EEditorModificationType
{
	EMT_Level,
	EMT_Count
};

class UEditorEngine : public UEngine
{
	MY_GENERATE_BODY(UEditorEngine)

	UEditorEngine()
		: UEngine(nullptr)
	{
	}

	UEditorEngine(D3DApp* Application)
		: UEngine(Application)
	{
	};
	~UEditorEngine() override;
	void               InitEngine() override;
	void               PostLoad() override;
	const std::string& GetDefaultMapName() override;

	void DrawEngineTitleBar();

	void SaveModifiedLevel();
	void SaveModifiedData();
	void HandleInput(UINT msg, WPARAM wParam, LPARAM lParam) override;

	void EditorModify(EEditorModificationType Type, std::function<void(bool)> Func = nullptr);

	bool IsEditorModify(EEditorModificationType Type) const
	{
		return EditorModificationTypes[static_cast<UINT>(Type)];
	}

protected:
	void CreateRenderThread() override;

private:
	CImage LogoImage;

	bool EditorModificationTypes[static_cast<UINT>(EEditorModificationType::EMT_Count)] = {false,};
	bool bEditorModified                                                                = false;
};

#endif