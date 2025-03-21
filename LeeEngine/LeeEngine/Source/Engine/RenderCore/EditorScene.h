// 03.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "RenderingThread.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/EditorClient/Panel/EditorClient.h"

enum class EDebugLogLevel
{
	DLL_Fatal, DLL_Error, DLL_Warning, DLL_Display, 
};
struct DebugText
{
	DebugText(){};
	DebugText(const std::string& Text, EDebugLogLevel Level)
	{
		this->Text = Text;
		this->Level = Level;
	}
	std::string Text;
	EDebugLogLevel Level;
	static std::map<EDebugLogLevel, ImVec4> Color;
};



#ifdef WITH_EDITOR
class FEditorScene : public FScene
{
public:
	FEditorScene();
	~FEditorScene() override {}

	std::unique_ptr<FEditorClient> EditorClient;


	// 디버깅 콘솔
	

	
	// ==================== ImGui ====================
	void InitLevelData() override;
	void BeginRenderFrame() override;
	void SetDrawScenePipeline(const float* ClearColor) override;
	void AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData) override;

	XMMATRIX GetViewMatrix() override;
	XMMATRIX GetProjectionMatrix() override;

	static void DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData);
	// ==================== IMGUI / IMGUIZMO ===================

	// 디버깅 콘솔 텍스트 추가 함수
	static void AddConsoleText_GameThread(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText);

	// 게임쓰레드 호출_월드 아웃라이너 내 액터 추가 함수 (register 시 호출)
	static void AddWorldOutlinerActor_GameThread(std::shared_ptr<AActor> NewActor);

	// ===================================================================
};

#endif



// ================================ 디버깅 로그 ================================
#ifdef WITH_EDITOR


#define MY_LOG(Category, DebugLogLevel, InDebugText) \
{\
	FEditorScene::AddConsoleText_GameThread(Category,DebugLogLevel,InDebugText);\
}
#else
#define MY_LOG(Category, DebugLogLevel, DebugText) ""	
#endif

#define XMFLOAT2_TO_TEXT(Data) std::format("x = {:.3f}, y = {:.3f}", Data.x,Data.y)

#define XMFLOAT3_TO_TEXT(Data) std::format("x = {:.3f}, y = {:.3f}, z = {:.3f}", Data.x,Data.y,Data.z)

#define XMFLOAT4_TO_TEXT(Data) std::format("x = {:.3f}, y = {:.3f}, z = {:.3f}, w = {:.3f}", Data.x,Data.y,Data.z, Data.w)

#define XMVECTOR_TO_TEXT(Data) std::format("x = {:.3f}, y = {:.3f}, z = {:.3f}, w = {:.3f}", DirectX::XMVectorGetX(Data),DirectX::XMVectorGetY(Data),DirectX::XMVectorGetZ(Data),DirectX::XMVectorGetW(Data))

// typeid와 __func__ 를 쓰기위해 매크로로 설정
#define GetFunctionName std::format("{}::{}", typeid(*this).name(), __func__)
