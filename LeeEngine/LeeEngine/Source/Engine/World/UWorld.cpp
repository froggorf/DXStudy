// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UWorld.h"

#include "Engine/AssetManager/AssetManager.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Engine/UEditorEngine.h"
#include "Engine/UEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/DirectX/Device.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/RenderCore/RenderingThread.h"

UWorld::UWorld()
{
}

UWorld::~UWorld()
{

}


void UWorld::Init()
{
	UObject::Init();

	//GEngine->AddImGuiRenderFunction(std::bind(&UWorld::ImguiRender_WorldOutliner, this));
	FScene::AddImGuiRenderFunction("World Outliner", []()
	{
		FScene::DrawWorldOutliner_RenderThread();
		FScene::DrawSelectActorDetail_RenderThread();
	});
	FScene::AddImGuizmoRenderFunction("Guizmo", []()
	{
		FScene::DrawImguizmoSelectedActor_RenderThread();
	});
	//GEngine->AddImGuiRenderFunction(std::bind(&UWorld::ImGuiRender_ActorDetail, this));

	//GEngine->AddImGuizmoRenderFunction(std::bind(&UWorld::ImGuizmoRender_SelectComponentGizmo, this));
}

void UWorld::PostLoad()
{
	UObject::PostLoad();

	if(PersistentLevel)
	{
		PersistentLevel->PostLoad();
	}
}

void UWorld::TickWorld(float DeltaSeconds)
{
	if(PersistentLevel)
	{

		PersistentLevel->TickLevel(DeltaSeconds);

	}
}

void UWorld::Tick()
{
}


void UWorld::SetPersistentLevel(const std::shared_ptr<ULevel>& NewLevel)
{
	FScene::InitSceneData_GameThread();
	PersistentLevel= NewLevel;
	PersistentLevel->Register();
}

void UWorld::LoadLevelInstanceByName(const std::string& NewLevelName)
{
	
}

void UWorld::AddLevel(const std::shared_ptr<ULevel>& NewLevel)
{
	// Set으로 설정하여 중복 추가 x
	Levels.emplace(NewLevel);
}
