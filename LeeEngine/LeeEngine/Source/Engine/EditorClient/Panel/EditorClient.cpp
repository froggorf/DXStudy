#include "CoreMinimal.h"
#ifdef WITH_EDITOR
#include "EditorClient.h"

#include "ImguiDebugConsole.h"
#include "ImguiEditorMainPanel.h"
#include "ImguiViewport.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/RenderCore/RenderingThread.h"

FEditorClient::FEditorClient(FScene* SceneData)
{
	ImguiPanels.resize(static_cast<UINT>(EImguiPanelType::IPT_Count));

	ImguiPanels[static_cast<UINT>(EImguiPanelType::IPT_MainPanel)]     = std::make_unique<FImguiEditorMainPanel>(SceneData);
	ImguiPanels[static_cast<UINT>(EImguiPanelType::IPT_LevelViewport)] = std::make_unique<FImguiLevelViewport>(SceneData);
	ImguiPanels[static_cast<UINT>(EImguiPanelType::IPT_DebugConsole)]  = std::make_unique<FImguiDebugConsole>(SceneData);
}

void FEditorClient::AddPanelCommand(const std::shared_ptr<FImguiPanelCommandData>& CommandData)
{
	if (0 == static_cast<UINT>(CommandData->PanelType))
	{
		MY_LOG("AddPanelCommand", EDebugLogLevel::DLL_Error, "Unknown PanelType");
		return;
	}

	if (FImguiPanel* Panel = ImguiPanels[static_cast<UINT>(CommandData->PanelType)].get())
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

	for (UINT Type = 0; Type < static_cast<UINT>(EImguiPanelType::IPT_Count); ++Type)
	{
		if (ImguiPanels[Type])
		{
			ImguiPanels[Type]->Draw();
		}
	}

	ImGui::PopFont();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// 0901) 해당 변수를 클래스 내에서 처리할 경우,
	// Imgui 한 프레임이 끝나기 전에 뷰포트 리사이즈를 진행하게 될 경우 Imgui 프레임의 마지막 부에 렌더링을 진행하게 되는데
	// 해당 리소스를 손실하게 되면서 리소스 누락이 발생해 프로그램이 터지게 됨
	// 따라서 모든 Imgui 렌더링이 작동하고나서 리사이즈가 되게 해야함
	if (FImguiLevelViewport* LevelViewport = dynamic_cast<FImguiLevelViewport*>(ImguiPanels[static_cast<int>(EImguiPanelType::IPT_LevelViewport)].get()))
	{
		if (LevelViewport->GetIsResizeEditorRenderTargetAtEndFrame())
		{
			const ImVec2& ResizeEditorRenderTargetSize = LevelViewport->GetResizeEditorRenderTargetSize();
			GDirectXDevice->ResizeEditorRenderTarget(ResizeEditorRenderTargetSize.x,ResizeEditorRenderTargetSize.y);
			LevelViewport->ResetIsResizeEditorRenderTargetAtEndFrame();
		}
		
	}
}
#endif