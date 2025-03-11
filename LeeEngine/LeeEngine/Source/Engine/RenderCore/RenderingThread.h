// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include <functional>
#include <concurrent_queue.h>
#include <ppltasks.h>
#include <thread>

#include "Engine/MyEngineUtils.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/DirectX/Device.h"
#include "Engine/GameFramework/AActor.h"
#include "ThirdParty/ImGui/backends/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/backends/imgui_impl_win32.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue

class USceneComponent;
class AActor;
class FPrimitiveSceneProxy;

struct FRenderTask
{
	UINT PrimitiveID;
	std::function<void(std::shared_ptr<class FScene>&)> CommandLambda;
	FRenderTask* Next;
	FRenderTask() : Next{ nullptr } {}
	FRenderTask(const FRenderTask& Other) : PrimitiveID{ Other.PrimitiveID }, CommandLambda{ Other.CommandLambda }, Next{ Other.Next } {}
};

class FRenderCommandPipe
{
private:
	static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>>& GetRenderCommandPipe()
	{
		static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>> RenderCommandPipe;
		return RenderCommandPipe;
	}
	
	//static std::unique_ptr<FRenderCommandPipe> RenderCommandPipe;
	//std::atomic<Node*> Head;
	//std::atomic<Node*> Tail;

public:
	static void Enqueue(std::function<void(std::shared_ptr<class FScene>&)>& CommandLambda)
	{
		// 다중 생성 기반 Queue
		std::shared_ptr<FRenderTask> NewNode = std::make_shared<FRenderTask>();
		//Node* NewNode = new Node();
		NewNode->CommandLambda = CommandLambda;

		GetRenderCommandPipe().push(NewNode);
		//Node* PrevHead = RenderCommandPipe->Head.exchange(NewNode);
		//PrevHead->Next= NewNode;
	}
	static bool Dequeue(std::shared_ptr<FRenderTask>& Result)
	{
		if(GetRenderCommandPipe().try_pop(Result))
		{
			return true;
		}

		return false;

		// 단일 소비 기반 queue
		//Node* PrevTail = Tail.load();
		//Node* Next = PrevTail->Next;
		//if (Next == nullptr)
		//{
		//	return false;
		//}
		//Result = *Next;
		//Tail.store(Next);
		//delete PrevTail;
		//return true;

	}

protected:
private:
public:
protected:
private:
	FRenderCommandPipe() = default;

	~FRenderCommandPipe() = default;
};

#define ENQUEUE_RENDER_COMMAND(Lambda) \
	std::function<void(std::shared_ptr<FScene>&)> temp = Lambda;\
		FRenderCommandPipe::Enqueue(temp);

inline bool bIsGameKill = false;
inline UINT RenderingThreadFrameCount = 0;

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

// 렌더링에 대한 정보를 가지고 있는 클래스 (씬 단위)
// 03.10 렌더링 쓰레드의 경우 단일 소비로 진행할 예정이므로
// 멀티쓰레드 동기화에 대한 처리 x
class FScene
{
public:
	// ==================== FPrimitiveSceneProxy ====================
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PrimitiveSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingAddSceneProxies;
	std::map<UINT, std::shared_ptr<FPrimitiveSceneProxy>> PendingDeleteSceneProxies;
	// ==================== FPrimitiveSceneProxy ====================

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
	// ==================== ImGui ====================

	bool bIsFrameStart;
protected:
public:

	static void KillRenderingThread()
	{
		bIsGameKill = true;
	}
	static void ShutdownImgui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	static void InitSceneData_GameThread()
	{ 
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				FScene::InitSceneData_RenderThread(SceneData);
			}
		)
	}

	static void InitSceneData_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData)
		{
			SceneData.reset();
		}
		SceneData = std::make_shared<FScene>();
	}

	static void BeginRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				BeginRenderFrame_RenderThread(SceneData);	
			}
		)
	}
	static void BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData->bIsFrameStart)
		{
			//MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
			return;
		}

		++RenderingThreadFrameCount;
		SceneData->bIsFrameStart = true;

#ifdef MYENGINE_BUILD_DEBUG || MYENGINE_BUILD_DEVELOPMENT
		for(const auto& Text : PendingAddDebugConsoleText)
		{
			DebugConsoleText.push_back(Text);
		}
		PendingAddDebugConsoleText.clear();
		for(const auto& NewOutlinerActor : PendingAddWorldOutlinerActors)
		{
			WorldOutlinerActors.push_back(NewOutlinerActor);
		}
		PendingAddWorldOutlinerActors.clear();
#endif

		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;
			
			MY_LOG("SceneProxy Add New Proxy - PrimitiveID = " + std::to_string(NewPrimitiveProxy.first), EDebugLogLevel::DLL_Display, "");
		}
		SceneData->PendingAddSceneProxies.clear();

		
	}

	static void EndRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				EndRenderFrame_RenderThread(SceneData);	
			}
		)
	}
	static void EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

		}
		SceneData->PendingAddSceneProxies.clear();
		SceneData->bIsFrameStart = false;
	}

	static void AddPrimitive_GameThread(UINT PrimitiveID, std::shared_ptr<FPrimitiveSceneProxy>& SceneProxy)
	{
		if(nullptr == SceneProxy)
		{
			return;
		}
		auto Lambda = 
			[PrimitiveID, SceneProxy](std::shared_ptr<FScene>& SceneData)
			{
				FScene::AddPrimitive_RenderThread(SceneData, PrimitiveID, SceneProxy);
			};
		ENQUEUE_RENDER_COMMAND(Lambda)
	}
	static void AddPrimitive_RenderThread(const std::shared_ptr<FScene>& SceneData, UINT PrimitiveID, const std::shared_ptr<FPrimitiveSceneProxy>& NewProxy)
	{
		if(!SceneData)
		{
			MY_LOG("SceneDataError", EDebugLogLevel::DLL_Error, "No SceneData");
			return;
		}
		SceneData->PendingAddSceneProxies[PrimitiveID] = NewProxy;
	}

	static void DrawScene_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
		{
			FScene::DrawScene_RenderThread(SceneData);
			
		})
		
	}

	static void DrawIMGUI_RenderThread(std::shared_ptr<FScene> SceneData)
	{
		/*if(ImGuiRenderFunctions.size() == 0)
		{
			return;
		}*/
		if(bIsGameKill)
		{
			return;
		}
		
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		// ImGui
		for(const auto& Func : ImGuiRenderFunctions)
		{
			Func.second();
		}


		//// ImGuizmo
		//ImGuizmo::BeginFrame();
		//ImGuiIO& io = ImGui::GetIO();
		//ImGuizmo::SetRect(0,0,io.DisplaySize.x,io.DisplaySize.y);
		//for(const auto& Func : ImGuizmoRenderFunctions)
		//{
		//	Func();
		//}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	static void DrawScene_RenderThread(std::shared_ptr<FScene> SceneData)
	{
		// 프레임 단위 세팅
		{
			GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
			GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());

			const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
			GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), clearColor);
			GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

			GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
			{
				// 셰이더 설정
				GDirectXDevice->GetDeviceContext()->VSSetShader(GDirectXDevice->GetStaticMeshVertexShader().Get(), nullptr, 0);
				GDirectXDevice->GetDeviceContext()->PSSetShader(GDirectXDevice->GetTexturePixelShader().Get(), nullptr, 0);
				// 상수버퍼 설정
				GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
				GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, GDirectXDevice->GetLightConstantBuffer().GetAddressOf());


				// 인풋 레이아웃
				GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
				GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// Frame 상수 버퍼 설정
				{
					// 뷰, 프로젝션 행렬
					{
						FrameConstantBuffer fcb;
						// TODO: 카메라 구현 시 수정
						XMFLOAT3 m_CameraPosition = XMFLOAT3(0.0f,5.0f,-5.0f);
						XMFLOAT3 m_CameraViewVector = XMFLOAT3(0.0f,-1.0f,1.0f);
						XMMATRIX ViewMat = XMMatrixLookToLH(XMLoadFloat3(&m_CameraPosition), XMLoadFloat3(&m_CameraViewVector), XMVectorSet(0.0f,1.0f,0.0f,0.0f));

						fcb.View = XMMatrixTranspose(ViewMat);
						// TODO: 카메라 구현 시 수정
						XMMATRIX ProjMat = XMMatrixPerspectiveFovLH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);
						fcb.Projection = XMMatrixTranspose(ProjMat);
						fcb.LightView = XMMatrixTranspose(XMMatrixIdentity());//m_LightView
						fcb.LightProj = XMMatrixTranspose(XMMatrixIdentity()); //m_LightProj
						GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetFrameConstantBuffer().Get(), 0, nullptr, &fcb, 0, 0);

					}

					// 라이팅 관련
					{
						LightFrameConstantBuffer lfcb;
						DirectionalLight TempDirectionalLight;
						TempDirectionalLight.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
						TempDirectionalLight.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
						TempDirectionalLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
						XMStoreFloat3(&TempDirectionalLight.Direction, XMVector3Normalize(XMVectorSet(0.57735f, -0.57735f, 0.57735f,0.0f)));
						lfcb.gDirLight = TempDirectionalLight;

						PointLight TempPointLight;
						TempPointLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
						TempPointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
						//m_PointLight.Position = XMFLOAT3(796.5f, 700.0f, 0.7549f);
						TempPointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
						TempPointLight.Range    = 0.0f;
						lfcb.gPointLight = TempPointLight;
						// Convert Spherical to Cartesian coordinates.
						lfcb.gEyePosW = XMFLOAT3{GEngine->Test_DeleteLater_GetCameraPosition()};
						GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetLightConstantBuffer().Get(),0,nullptr,&lfcb, 0,0);
					}
				}




				// Sampler State 설정
				GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, GDirectXDevice->GetSamplerState().GetAddressOf());

				//if(CurrentWorld)
				//{
				//	CurrentWorld->TestDrawWorld();
				//}
				//DrawImGui();

			}
		}


		DrawIMGUI_RenderThread(SceneData);
		
		HR(GDirectXDevice->GetSwapChain()->Present(0, 0));

		// SceneProxy Render
		EndRenderFrame_RenderThread(SceneData);
	}


	// ==================== IMGUI / IMGUIZMO ===================
	static void AddImGuiRenderFunction(const std::string& Name, const std::function<void()>& NewRenderFunction)
	{
		ImGuiRenderFunctions[Name] = NewRenderFunction;
	}
	static void AddConsoleText_GameThread(const std::string& Category, EDebugLogLevel DebugLevel, const std::string& InDebugText)
	{
#ifdef MYENGINE_BUILD_DEBUG || MYENGINE_BUILD_DEVELOPMENT

		std::string NewText = Category + " : " + InDebugText;
		DebugText NewDebugText{NewText, DebugLevel};
		ENQUEUE_RENDER_COMMAND([NewDebugText](std::shared_ptr<FScene>& SceneData)
			{
				FScene::PendingAddDebugConsoleText.push_back(NewDebugText);
			}
		)
#endif
	}
	
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

	static void DrawDebugConsole_RenderThread()
	{
		ImGui::Begin("Debug Console");

		// EditBox
		char* CurrentText = DebugConsoleSearchText.data();
		ImGui::Text("Search: ");
		ImGui::SameLine();
		if(ImGui::InputText(" ",CurrentText,100))
		{
			DebugConsoleSearchText = CurrentText;
			SearchDebugConsole_RenderThread();
		}

		if(ImGui::BeginListBox(" ", ImVec2(-FLT_MIN, -FLT_MIN)))
		{
			// 디버그 리스트 박스의 맨 아래를 볼 시 맨 아래로 고정
			bool bIsFixListBox = ImGui::GetScrollMaxY() == ImGui::GetScrollY();


			// 검색 중 체크
			bool bWhileSearching = DebugConsoleSearchText.size() != 0;
			if(bWhileSearching)
			{
				for(const DebugText& Text : SearchingDebugConsoleText)
				{
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());
				}	
			}
			// 검색 아닐 시
			else
			{
				for(const DebugText& Text : DebugConsoleText)
				{
					ImGui::TextColored(DebugText::Color[Text.Level], Text.Text.data());

				}	
			}

			// 리스트 맨 아래였을 시 고정
			if(bIsFixListBox)
			{
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::EndListBox();

		}


		ImGui::End();
	}

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

	static void AddWorldOutlinerActor_GameThread(std::shared_ptr<AActor> NewActor)
	{
		ENQUEUE_RENDER_COMMAND([NewActor](std::shared_ptr<FScene>& Dummy)
		{
			FScene::AddWorldOutlinerActor_RenderThread(NewActor);
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

	static void SelectActorFromWorldOutliner_RenderThread(const std::shared_ptr<AActor>& NewSelectedActor)
	{
		SelectActorComponents.clear();
		SelectActorComponentNames.clear();
		CurrentSelectedComponentIndex = 0;
		CurrentSelectedActor = NewSelectedActor;
		FindComponentsAndNamesFromActor_RenderThread(CurrentSelectedActor->GetRootComponent(), 0);
	}

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

	// ===================================================================
public:
protected:
private:
};


// 엔진 초기화 시 생성되며
// RenderCommandPipe에 존재하는 Command를 계속해서 수행하는 클래스
class FRenderCommandExecutor
{
public:
	static void Execute()
	{
		std::shared_ptr<FRenderTask> Task;
		while(true)
		{
			if(bIsGameKill)
			{
				return;
			}

			if(FRenderCommandPipe::Dequeue(Task))
			{
				Task->CommandLambda(CurrentSceneData);
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	//// 반드시 RenderCommandPipe를 통해 호출해야함
	//// ENQUE_RENDER_COMMAND(...
	//static void InitNewSceneData()
	//{
	//	if(CurrentSceneData)
	//	{
	//		CurrentSceneData.release();
	//	}

	//	CurrentSceneData = std::make_unique<FScene>();
	//}
protected:
private:
public:
protected:
private:
	static std::shared_ptr<FScene> CurrentSceneData;
};