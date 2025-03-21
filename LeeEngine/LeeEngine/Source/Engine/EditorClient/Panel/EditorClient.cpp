#include "EditorClient.h"


#include "ImguiDebugConsole.h"
#include "ImguiViewport.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/RenderCore/RenderingThread.h"


FEditorClient::FEditorClient(FScene* SceneData)
{
	ImguiPanels.resize(static_cast<UINT>(EImguiPanelType::IPT_Count));

	//ImguiPanels[static_cast<UINT>(EImguiPanelType::IPT_SceneViewport)] = std::make_unique<FImguiViewport>(SceneData);
	ImguiPanels[static_cast<UINT>(EImguiPanelType::IPT_DebugConsole)] = std::make_unique<FImguiDebugConsole>(SceneData);
}

void FEditorClient::AddPanelCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData)
{
	if(0 == static_cast<UINT>(CommandData->PanelType))
	{
		MY_LOG("AddPanelCommand", EDebugLogLevel::DLL_Error, "Unknown PanelType");
		return;
	}

	if(FImguiPanel* Panel = ImguiPanels[static_cast<UINT>(CommandData->PanelType)].get())
	{
		Panel->ExecuteCommand(CommandData);
	}
}



void FEditorClient::Draw()
{
	//========== IMGUI ==========
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();



	static ImFont* RobotoFont = ImGui::GetIO().Fonts->Fonts[0];
	ImGui::PushFont(RobotoFont);

	for(UINT Type = 0; Type < static_cast<UINT>(EImguiPanelType::IPT_Count); ++Type)
	{
		if(ImguiPanels[Type])
		{
			ImguiPanels[Type]->Draw();	
		}
	}


	
	ImGui::PopFont();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::EndFrame();

	
}
