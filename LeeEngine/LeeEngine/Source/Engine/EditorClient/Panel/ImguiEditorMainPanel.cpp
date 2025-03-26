#include "ImguiEditorMainPanel.h"

#include "Engine/AssetManager/AssetManager.h"

FImguiEditorMainPanel::FImguiEditorMainPanel(FScene* SceneData)
	:FImguiPanel(SceneData)
{
	std::string PlayIconPath = GEngine->GetEngineDirectory()+"/Content/Editor/Icon/PlayIcon.png";
	AssetManager::LoadTextureFromFile(std::wstring{PlayIconPath.begin(),PlayIconPath.end()} , GDirectXDevice->GetDevice(),PlayIcon);
}

void FImguiEditorMainPanel::Draw()
{
	//RECT ClientRect;
	//GetClientRect(GEngine->GetWindow(), &ClientRect);
	////ClientRect.top += WindowTitleBarHeight;
	//ImGui::SetNextWindowPos(ImVec2(ClientRect.left,ClientRect.top));
	//ImGui::SetNextWindowSize(ImVec2{ static_cast<float>(ClientRect.right-ClientRect.left),static_cast<float>( ClientRect.bottom- ClientRect.top)});
	//
	if(ImGui::Begin("  ", nullptr, ImGuiWindowFlags_NoTitleBar))
	{
		ImVec2 WindowSize = ImGui::GetWindowSize();
		MY_LOG("Test", EDebugLogLevel::DLL_Warning, XMFLOAT2_TO_TEXT(WindowSize));
		if(PlayIcon)
		{
			float PlayButtonSize=  WindowSize.y - 20 - 20 - 10;
			if(ImGui::ImageButton(" ", (void*)PlayIcon.Get(), ImVec2{PlayButtonSize,PlayButtonSize}))
			{
			}
		}
		

		ImGui::End();
	}
}
