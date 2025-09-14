// 03.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "EditorScene.h"

#include "Engine/EditorClient/Panel/ImguiDebugConsole.h"
#include "Engine/EditorClient/Panel/ImguiViewport.h"

#ifdef WITH_EDITOR

FEditorScene::FEditorScene()
{
	EditorClient = std::make_unique<FEditorClient>(this);
}

void FEditorScene::InitSceneData_GameThread()
{
	ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData) 
	{
		SceneData = std::make_shared<FEditorScene>();
		std::shared_ptr<FRenderTask> DummyTask;
		// 기존에 남아있는 렌더 명령어 모두 Dequeue
		while(FRenderCommandPipe::Dequeue(DummyTask))
		{
			DummyTask->CommandLambda(SceneData);
		}
		FViewMatrices EditorViewMatrices;
		auto CommandData = std::make_shared<FImguiLevelViewportCommandData>();
		CommandData->PanelType = EImguiPanelType::IPT_LevelViewport;
		CommandData->CommandType = ELevelViewportCommandType::LVCT_ChangeLevelInitialize;
		std::dynamic_pointer_cast<FEditorScene>(SceneData)->GetEditorClient()->AddPanelCommand(CommandData);
	})
}

void FEditorScene::SetDrawScenePipeline_HDR_MiddleStep()
{
	GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_HDR)->OMSet();
	GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_HDR)->ClearRenderTarget();	
}

void FEditorScene::SetDrawScenePipeline()
{
	GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->OMSet();
	GDirectXDevice->GetMultiRenderTarget(EMultiRenderTargetType::Editor_Main)->ClearRenderTarget();	
}

void FEditorScene::SetRSViewport()
{
	// 에디터 뷰포트 사이즈 설정하기
	{
		auto CommandData         = std::make_shared<FImguiLevelViewportCommandData>();
		CommandData->PanelType   = EImguiPanelType::IPT_LevelViewport;
		CommandData->CommandType = ELevelViewportCommandType::LVCT_SetViewportSizeToEditorViewportSize;
		CommandData->ViewMatrices = &ViewMatrices;
		EditorClient->AddPanelCommand(CommandData);
	}
}

void FEditorScene::AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData)
{
	GDirectXDevice->SetDefaultViewPort();
	EditorClient->Draw();
}

const FViewMatrices& FEditorScene::GetViewMatrices() 
{
	static UINT LastUpdateFrame = -1;
	if (LastUpdateFrame != RenderingThreadFrameCount)
	{
		LastUpdateFrame = RenderingThreadFrameCount;

		EditorViewMatrices.UpdateViewMatrix(GetViewMatrix());
		EditorViewMatrices.UpdateProjectionMatrix(GetProjectionMatrix());
	}
	

	return EditorViewMatrices;
}

XMMATRIX FEditorScene::GetViewMatrix()
{
	if (GEngine->bGameStart)
	{
		return FScene::GetViewMatrix();
	}
	else
	{
		auto          CommandData = std::make_shared<FImguiLevelViewportCommandData>();
		CommandData->PanelType    = EImguiPanelType::IPT_LevelViewport;
		CommandData->CommandType  = ELevelViewportCommandType::LVCT_GetEditorViewMatrices;
		CommandData->ViewMatrices = &ViewMatrices;
		EditorClient->AddPanelCommand(CommandData);

		return ViewMatrices.GetViewMatrix();	
	}
	
}

XMMATRIX FEditorScene::GetProjectionMatrix()
{
	return ViewMatrices.GetProjectionMatrix();
}

void FEditorScene::DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData)
{
}

void FEditorScene::AddConsoleText_GameThread(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText)
{
	std::string NewText = Category + " : " + InDebugText;
	DebugText   NewDebugText{NewText, DebugLevel};
	auto        Lambda = [NewDebugText](std::shared_ptr<FScene>& SceneData)
	{
		if (std::shared_ptr<FEditorScene> EditorSceneData = std::dynamic_pointer_cast<FEditorScene>(SceneData))
		{
			auto CommandData         = std::make_shared<FImguiDebugConsoleCommandData>();
			CommandData->PanelType   = EImguiPanelType::IPT_DebugConsole;
			CommandData->CommandType = EDebugConsoleCommandType::DCCT_AddConsoleText;
			CommandData->DebugText   = NewDebugText;
			EditorSceneData->EditorClient->AddPanelCommand(CommandData);
		}
	};
	ENQUEUE_RENDER_COMMAND(Lambda);
}

void FEditorScene::AddWorldOutlinerActor_GameThread(std::shared_ptr<AActor> NewActor)
{
	auto Lambda = [NewActor](std::shared_ptr<FScene>& SceneData)
	{
		if (std::shared_ptr<FEditorScene> EditorSceneData = std::dynamic_pointer_cast<FEditorScene>(SceneData))
		{
			auto CommandData                = std::make_shared<FImguiLevelViewportCommandData>();
			CommandData->PanelType          = EImguiPanelType::IPT_LevelViewport;
			CommandData->CommandType        = ELevelViewportCommandType::LVCT_AddActorToWorldOutliner;
			CommandData->NewPendingAddActor = NewActor;
			EditorSceneData->EditorClient->AddPanelCommand(CommandData);
		}
	};

	ENQUEUE_RENDER_COMMAND(Lambda);
}

#endif
