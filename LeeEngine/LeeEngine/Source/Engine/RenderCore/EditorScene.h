// 03.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "RenderingThread.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/SceneView.h"
#include "Engine/EditorClient/Panel/EditorClient.h"

enum class EDebugLogLevel
{
	DLL_Fatal, DLL_Error, DLL_Warning, DLL_Display, 
};
struct DebugText
{
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
	// ==================== ImGui ====================
	static std::unordered_map<std::string,std::function<void()>> ImGuiRenderFunctions;
	static std::unordered_map<std::string,std::function<void()>> ImGuizmoRenderFunctions;


	// 디버깅 콘솔
	static std::vector<DebugText> DebugConsoleText;
	static std::vector<DebugText> PendingAddDebugConsoleText;
	static std::vector<DebugText> SearchingDebugConsoleText;
	static std::string DebugConsoleSearchText;

	// 월드 아웃라이너
	static std::vector<std::shared_ptr<AActor>> WorldOutlinerActors;
	static std::vector<std::shared_ptr<AActor>> PendingAddWorldOutlinerActors;
	static std::shared_ptr<AActor> CurrentSelectedActor;
	static std::vector<std::shared_ptr<USceneComponent>> SelectActorComponents;
	static std::vector<std::string> SelectActorComponentNames;
	static int CurrentSelectedComponentIndex;


	// 에디터 ViewPort
	static bool bResizeEditorRenderTargetAtEndFrame;
	static ImVec2 ResizeEditorRenderTargetSize;
	static FViewMatrices EditorViewMatrices;
	// ==================== ImGui ====================
	void InitLevelData() override;
	void BeginRenderFrame() override;
	void SetDrawScenePipeline(const float* ClearColor) override;
	void AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData) override;

	XMMATRIX GetViewMatrix() override;
	XMMATRIX GetProjectionMatrix() override;

	static void DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData);
	// ==================== IMGUI / IMGUIZMO ===================
	// ImGUI렌더링 함수(람다) 추가 함수


	static void AddImGuiRenderFunction(const std::string& Name, const std::function<void()>& NewRenderFunction)	{ ImGuiRenderFunctions[Name] = NewRenderFunction; }
	static void AddImGuizmoRenderFunction(const std::string& Name, const std::function<void()>& NewRenderFunction) { ImGuizmoRenderFunctions[Name] = NewRenderFunction; }
	// 디버깅 콘솔 텍스트 추가 함수
	static void AddConsoleText_GameThread(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText);
	// Search Debug Console
	static void SearchDebugConsole_RenderThread()
	{
		SearchingDebugConsoleText.clear();

		if(DebugConsoleSearchText.size() == 0)
		{
			return;
		}

		for(const auto& Text : DebugConsoleText)
		{
			const std::string& LogText = Text.Text;
			if(LogText.contains(DebugConsoleSearchText))
			{
				SearchingDebugConsoleText.push_back(Text);
			}
		}
	}
	// 렌더링 Debug Console
	static void DrawDebugConsole_RenderThread();

	// 월드 아웃라이너 렌더링 함수
	static void DrawWorldOutliner_RenderThread()
	{
		static int CurrentItem = -1;
		ImGui::Begin("World Outliner", nullptr);

		if (ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,-FLT_MIN))) {
			int ActorCount = WorldOutlinerActors.size();
			for (int i = 0; i < ActorCount; i++) {
				const bool is_selected = (CurrentItem== i);
				if (ImGui::Selectable(WorldOutlinerActors[i]->GetName().c_str(), is_selected)) {
					CurrentItem = i;
					SelectActorFromWorldOutliner_RenderThread(WorldOutlinerActors[i]);
				}

				// 선택된 항목에 대한 포커스 처리
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}

		ImGui::End();

	}
	// 월드 아웃라이너_액터 선택시 디테일 패널 출력
	static void DrawSelectActorDetail_RenderThread()
	{
		ImGui::Begin("Detail");
		if(CurrentSelectedActor)
		{
			// 컴퍼넌트들 렌더링
			{
				if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN,200.0f)))
				{
					int ComponentCount = SelectActorComponentNames.size();
					for (int i = 0; i < ComponentCount; i++) {
						const bool is_selected = (CurrentSelectedComponentIndex == i);
						if (ImGui::Selectable(SelectActorComponentNames[i].data(), is_selected)) {
							CurrentSelectedComponentIndex = i;
							// TODO : Select Component Action
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndListBox();
				}
			}

		}
		else
		{
			ImGui::Text("No Select");
		}
		ImGui::End();
	}
	// 게임쓰레드 호출_월드 아웃라이너 내 액터 추가 함수 (register 시 호출)
	static void AddWorldOutlinerActor_GameThread(std::shared_ptr<AActor> NewActor)
	{
		ENQUEUE_RENDER_COMMAND([NewActor](std::shared_ptr<FScene>& Dummy)
			{

				FEditorScene::AddWorldOutlinerActor_RenderThread(NewActor);
			})
	}
	//static void AddWorldOutlinerActor_GameThread(std::vector<std::shared_ptr<AActor>>& NewActors)
	//{
	//	ENQUEUE_RENDER_COMMAND([NewActors](std::shared_ptr<FScene>& Dummy)
	//		{
	//			
	//			FScene::AddWorldOutlinerActor_RenderThread(NewActors);
	//		})
	//}

	static void AddWorldOutlinerActor_RenderThread(std::shared_ptr<AActor> NewActor)
	{
		PendingAddWorldOutlinerActors.push_back(NewActor);
	}
	//static void AddWorldOutlinerActor_RenderThread(std::vector<std::shared_ptr<AActor>> NewActors)
	//{
	//	PendingAddWorldOutlinerActors.reserve(PendingAddWorldOutlinerActors.capacity() + NewActors.size());
	//	for(const auto& NewActor : NewActors)
	//	{
	//		PendingAddWorldOutlinerActors.push_back(NewActor);
	//	}
	//	
	//}
	// 월드 아웃라이너의 액터를 선택 시 호출되는 함수
	static void SelectActorFromWorldOutliner_RenderThread(const std::shared_ptr<AActor>& NewSelectedActor)
	{
		SelectActorComponents.clear();
		SelectActorComponentNames.clear();
		CurrentSelectedComponentIndex = 0;
		CurrentSelectedActor = NewSelectedActor;
		FindComponentsAndNamesFromActor_RenderThread(CurrentSelectedActor->GetRootComponent(), 0);
	}
	// 선택된 월드아웃라이너 액터의 컴퍼넌트 계층구조를 찾는 함수
	static void FindComponentsAndNamesFromActor_RenderThread(const std::shared_ptr<USceneComponent>& TargetComponent, int CurrentHierarchyDepth)
	{
		if(!CurrentSelectedActor)
		{
			return;
		}

		std::string HierarchyTabString;
		HierarchyTabString.reserve(2*CurrentHierarchyDepth);
		for(int i = 0; i < CurrentHierarchyDepth; ++i)
		{
			HierarchyTabString += "  ";
		}

		std::string TargetComponentName = HierarchyTabString + TargetComponent->GetName();

		SelectActorComponents.push_back(TargetComponent);
		SelectActorComponentNames.push_back(TargetComponentName);

		const std::vector<std::shared_ptr<USceneComponent>>& TargetComponentChildren = TargetComponent->GetAttachChildren();
		for(const auto& ChildComponent : TargetComponentChildren)
		{
			FindComponentsAndNamesFromActor_RenderThread(ChildComponent, CurrentHierarchyDepth+1);
		}
	}

	static void DrawImguizmoSelectedActor_RenderThread(float AspectRatio);
	static void DrawImGuiScene_RenderThread();
	static void EditorCameraMove(XMFLOAT3 MoveDelta, XMFLOAT2 MouseDelta);

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
