// 03.26
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "ImguiPanel.h"

class FScene;

class FImguiEditorMainPanel : public FImguiPanel
{
public:
	FImguiEditorMainPanel(FScene* SceneData);
	~FImguiEditorMainPanel() override = default;

	void Draw() override;

	void ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData) override { }

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PlayIcon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> StopIcon;
};
