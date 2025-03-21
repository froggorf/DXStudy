#include "EditorClient.h"


#include "ImguiViewport.h"
#include "Engine/RenderCore/RenderingThread.h"


FEditorClient::FEditorClient(class FScene* SceneData)
{
	ImguiPanels.reserve(static_cast<UINT>(EImguiPanelType::IPT_Count));

	ImguiPanels[(UINT)EImguiPanelType::IPT_SceneViewport] = std::make_unique<FImguiViewport>(SceneData);
	ImguiPanels[(UINT)EImguiPanelType::IPT_DebugConsole] = std::make_unique<FImguiViewport>(SceneData);
}

void FEditorClient::Draw()
{
	for(UINT Type = 0; Type < static_cast<UINT>(EImguiPanelType::IPT_Count); ++Type)
	{
		ImguiPanels[Type]->Draw();
	}
}
