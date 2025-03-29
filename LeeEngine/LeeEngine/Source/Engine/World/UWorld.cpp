// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UWorld.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/UEditorEngine.h"
#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"
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
#ifdef WITH_EDITOR
	/*FEditorScene::AddImGuiRenderFunction("World Outliner", []()
	{
		FEditorScene::DrawWorldOutliner_RenderThread();
		FEditorScene::DrawSelectActorDetail_RenderThread();
	});*/
#endif
}

void UWorld::PostLoad()
{
	UObject::PostLoad();

	if(PersistentLevel)
	{
		PersistentLevel->PostLoad();
	}
}

void UWorld::BeginPlay()
{
	UObject::BeginPlay();

	if(!PersistentLevel)
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "Not valid PersistentLevel");
		return;
	}
	PersistentLevel->BeginPlay();
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
	PersistentLevel= NewLevel;
	PersistentLevel->Register();
#ifdef WITH_EDITOR
	// 타이틀 바 내 현재 레벨 이름 변경
	SendMessage(GEditorEngine->GetWindow(), WM_NCPAINT,true,0);
#endif
}

void UWorld::LoadLevelInstanceByName(const std::string& NewLevelName)
{
	
}

void UWorld::AddLevel(const std::shared_ptr<ULevel>& NewLevel)
{
	// Set으로 설정하여 중복 추가 x
	Levels.emplace(NewLevel);
}
