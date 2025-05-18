#include "CoreMinimal.h"
#include "ImguiEditorMainPanel.h"

#include "Engine/AssetManager/AssetManager.h"

FImguiEditorMainPanel::FImguiEditorMainPanel(FScene* SceneData)
	: FImguiPanel(SceneData)
{
	std::string PlayIconPath = GEngine->GetEngineDirectory() + "/Content/Editor/Icon/PlayIcon.png";
	AssetManager::LoadTextureFromFile(std::wstring{PlayIconPath.begin(), PlayIconPath.end()},
									GDirectXDevice->GetDevice(), PlayIcon);
	std::string StopIconPath = GEngine->GetEngineDirectory() + "/Content/Editor/Icon/StopIcon.png";
	AssetManager::LoadTextureFromFile(std::wstring{StopIconPath.begin(), StopIconPath.end()},
									GDirectXDevice->GetDevice(), StopIcon);
}

void FImguiEditorMainPanel::Draw()
{
	//RECT ClientRect;
	//GetClientRect(GEngine->GetWindow(), &ClientRect);
	////ClientRect.top += WindowTitleBarHeight;
	//ImGui::SetNextWindowPos(ImVec2(ClientRect.left,ClientRect.top));
	//ImGui::SetNextWindowSize(ImVec2{ static_cast<float>(ClientRect.right-ClientRect.left),static_cast<float>( ClientRect.bottom- ClientRect.top)});
	//
	if (ImGui::Begin("  ##MainPanel", nullptr, ImGuiWindowFlags_NoTitleBar))
	{
		ImVec2 WindowSize     = ImGui::GetWindowSize();
		float  TitleBarHeight = 30.0f;
		if (PlayIcon)
		{
			ImGui::SetCursorPos(ImVec2{500.0f, TitleBarHeight + 5.0f});
			float PlayButtonSize = WindowSize.y - TitleBarHeight - 20 - 5;
			if (GEditorEngine->bGameStart)
			{
				ImGui::ImageButton(" ", reinterpret_cast<ImTextureID>(StopIcon.Get()),
									ImVec2{PlayButtonSize, PlayButtonSize});
			}
			else
			{
				if (ImGui::ImageButton(" ", reinterpret_cast<ImTextureID>(PlayIcon.Get()),
										ImVec2{PlayButtonSize, PlayButtonSize}))
				{
					ENQUEUE_IMGUI_COMMAND([]() {
						//MY_LOG("TEST", EDebugLogLevel::DLL_Warning, "Start Game");
						GEngine->GameStart(); })
				}
			}
		}

		ImGui::End();
	}
}
