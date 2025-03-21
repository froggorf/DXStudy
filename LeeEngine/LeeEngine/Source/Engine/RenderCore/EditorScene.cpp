// 03.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "EditorScene.h"

#include "Engine/EditorClient/Panel/ImguiDebugConsole.h"
#include "Engine/EditorClient/Panel/ImguiViewport.h"


#ifdef WITH_EDITOR




FEditorScene::FEditorScene()
{
	EditorClient = std::make_unique<FEditorClient>(this);
}



void FEditorScene::InitLevelData()
{
	FScene::InitLevelData();

	std::shared_ptr<FImguiLevelViewportCommandData> CommandData = std::make_shared<FImguiLevelViewportCommandData>();
	CommandData->PanelType = EImguiPanelType::IPT_LevelViewport;
	CommandData->CommandType = ELevelViewportCommandType::LVCT_ClearCurrentLevelData;
	EditorClient->AddPanelCommand(CommandData);
	
	
}

void FEditorScene::BeginRenderFrame()
{
	FScene::BeginRenderFrame();
}

void FEditorScene::SetDrawScenePipeline(const float* ClearColor)
{
	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetEditorRenderTargetView().GetAddressOf(), GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->ClearRenderTargetView(GDirectXDevice->GetEditorRenderTargetView().Get(),ClearColor);

	// 에디터 뷰포트 사이즈 설정하기
	{
		std::shared_ptr<FImguiLevelViewportCommandData> CommandData = std::make_shared<FImguiLevelViewportCommandData>();
		CommandData->PanelType = EImguiPanelType::IPT_LevelViewport;
		CommandData->CommandType = ELevelViewportCommandType::LVCT_SetViewportSizeToEditorViewportSize;
		EditorClient->AddPanelCommand(CommandData);
	}
	
}

void FEditorScene::AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData)
{
	FScene::AfterDrawSceneAction(SceneData);

	GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
	GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());
	GDirectXDevice->SetDefaultViewPort();

	EditorClient->Draw();
}

XMMATRIX FEditorScene::GetViewMatrix()
{
	FViewMatrices EditorViewMatrices;
	std::shared_ptr<FImguiLevelViewportCommandData> CommandData = std::make_shared<FImguiLevelViewportCommandData>();
	CommandData->PanelType = EImguiPanelType::IPT_LevelViewport;
	CommandData->CommandType = ELevelViewportCommandType::LVCT_GetEditorViewMatrices;
	CommandData->ViewMatrices = &EditorViewMatrices;
	EditorClient->AddPanelCommand(CommandData);
	return EditorViewMatrices.GetViewMatrix();
}

XMMATRIX FEditorScene::GetProjectionMatrix()
{
	// TODO: 수정 예정 03/20
	return FScene::GetProjectionMatrix();//ViewMatrices.GetProjectionMatrix();
}

void FEditorScene::DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData)
{
}

void FEditorScene::AddConsoleText_GameThread(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText)
{
	std::string NewText = Category + " : " + InDebugText;
	DebugText NewDebugText{NewText, DebugLevel};
	ENQUEUE_RENDER_COMMAND([NewDebugText](std::shared_ptr<FScene>& SceneData)
		{
			//FEditorScene::PendingAddDebugConsoleText.push_back(NewDebugText);
			if(std::shared_ptr<FEditorScene> EditorSceneData = std::dynamic_pointer_cast<FEditorScene>(SceneData))
			{
				std::shared_ptr<FImguiDebugConsoleCommandData> CommandData = std::make_shared<FImguiDebugConsoleCommandData>();
				CommandData->PanelType = EImguiPanelType::IPT_DebugConsole;
				CommandData->CommandType = EDebugConsoleCommandType::DCCT_AddConsoleText;
				CommandData->DebugText = NewDebugText;
				EditorSceneData->EditorClient->AddPanelCommand(CommandData);
			}
		}
	)
}

void FEditorScene::AddWorldOutlinerActor_GameThread(std::shared_ptr<AActor> NewActor)

{
	ENQUEUE_RENDER_COMMAND([NewActor](std::shared_ptr<FScene>& SceneData)
		{
			if(std::shared_ptr<FEditorScene> EditorSceneData = std::dynamic_pointer_cast<FEditorScene>(SceneData))
			{
				std::shared_ptr<FImguiLevelViewportCommandData> CommandData = std::make_shared<FImguiLevelViewportCommandData>();
				CommandData->PanelType = EImguiPanelType::IPT_LevelViewport;
				CommandData->CommandType = ELevelViewportCommandType::LVCT_AddActorToWorldOutliner;
				CommandData->NewPendingAddActor = NewActor;
				EditorSceneData->EditorClient->AddPanelCommand(CommandData);
			}
		})
}


#endif