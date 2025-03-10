// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEditorEngine.h"
#include <fstream>
#include <memory>

#include "Engine/AssetManager/AssetManager.h"
#include "imgui_internal.h"
#include "World/UWorld.h"


std::shared_ptr<UEditorEngine> GEditorEngine = nullptr;

std::map<EDebugLogLevel, ImVec4> DebugText::Color = std::map<EDebugLogLevel, ImVec4>
{
	{EDebugLogLevel::DLL_Error, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Fatal, ImVec4(0.77f,0.26f,0.26f,1.0f)},
	{EDebugLogLevel::DLL_Display, ImVec4(0.8f,0.8f,0.8f,1.0f)},
	{EDebugLogLevel::DLL_Warning, ImVec4(0.95f,0.73f,0.125f,1.0f)},
};

void UEditorEngine::InitEngine()
{
	UEngine::InitEngine();

	//AddImGuiRenderFunction(std::bind(&UEditorEngine::DrawDebugConsole, this));
	FScene::AddImGuiRenderFunction(
		"DebugConsole",
		[]()
		{
			FScene::DrawDebugConsole_RenderThread();
		}
	);
	MY_LOG("Init", EDebugLogLevel::DLL_Display, "UEditorEngine init");
	
}

void UEditorEngine::PostLoad()
{
	UEngine::PostLoad();

	// Default Engine Map
	//if(GetWorld())
	//{
	//	//std::shared_ptr<ULevel> NewLevel = std::make_shared<ULevel>(*AssetManager::ReadMyAsset<ULevel>(EngineData["EditorStartupMap"]));
	//	//GetWorld()->SetPersistentLevel(NewLevel);
	//	std::shared_ptr<ULevel> NewLevel = std::make_shared<ULevel>(GetWorld());
	//	GetWorld()->SetPersistentLevel(NewLevel);
	//}

}

const std::string& UEditorEngine::GetDefaultMapName()
{
	return EngineData["EditorStartupMap"];
}

