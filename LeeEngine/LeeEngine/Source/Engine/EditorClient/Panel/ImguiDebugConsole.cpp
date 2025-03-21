#include "ImguiDebugConsole.h"

#include "Engine/RenderCore/EditorScene.h"

FImguiDebugConsole::FImguiDebugConsole(FScene* SceneData)
	: FImguiPanel(SceneData)
{
}

void FImguiDebugConsole::Draw()
{
	if(ImGui::Begin("TEST"))
	{
		
		ImGui::End();
	}
}

void FImguiDebugConsole::ExecuteCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData)
{
	//if(CommandData->IsOfType(FImguiDebugConsoleCommandData::CommandPanelTypeID))
	//{
	//	//FImguiPanelCommandData* Data = CommandData.get();
	//	//FImguiDebugConsoleCommandData* DebugConsoleData = static_cast<FImguiDebugConsoleCommandData*>(Data);
	//	//std::cout <<  DebugConsoleData->DebugText->Text << std::endl;
	//}
}