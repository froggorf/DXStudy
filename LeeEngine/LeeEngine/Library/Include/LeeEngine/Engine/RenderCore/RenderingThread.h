// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include <functional>
#include <concurrent_queue.h>
#include <ppltasks.h>
#include <thread>

#include "ImGUIActionTask.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/DirectX/Device.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/SceneProxy/FPrimitiveSceneProxy.h"
#include "ThirdParty/ImGui/backends/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/backends/imgui_impl_win32.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue

class USceneComponent;
class AActor;

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
inline std::atomic<UINT> RenderingThreadFrameCount = 0;

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
	// 엔진 종료 시 렌더링 쓰레드를 죽이는 함수
	static void KillRenderingThread()
	{
		bIsGameKill = true;
	}
	// ImGUI 종료 함수
	static void ShutdownImgui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	// 게임쓰레드 _ 레벨마다 가진 씬 데이터를 Init하는 함수 
	static void InitSceneData_GameThread()
	{ 
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				FScene::InitSceneData_RenderThread(SceneData);
			}
		)
	}
	// InitSceneData_GameThread()
	static void InitSceneData_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		if(SceneData)
		{
			SceneData.reset();
		}
		SceneData = std::make_shared<FScene>();
	}

	// 렌더쓰레드 프레임 시작 알림 함수
	// 게임쓰레드 시작 시 호출
	static void BeginRenderFrame_GameThread(UINT GameThreadFrameCount)
	{
		ENQUEUE_RENDER_COMMAND([GameThreadFrameCount](std::shared_ptr<FScene>& SceneData)
			{
				BeginRenderFrame_RenderThread(SceneData, GameThreadFrameCount);	
			}
		)
	}
	static void BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount)
	{
		if(SceneData->bIsFrameStart)
		{
			//MY_LOG("RenderCommand", EDebugLogLevel::DLL_Error,"RenderThread is already start");
			return;
		}

		RenderingThreadFrameCount = GameThreadFrameCount;
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

	// 렌더 쓰레드 프레임 종료 함수 (Draw에서 호출)
	static void EndRenderFrame_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData)
			{
				EndRenderFrame_RenderThread(SceneData);	
			}
		)
	}

	// 새로운 UPrimitiveComponent 생성 후 register 시 렌더링 쓰레드에게 알리는 함수
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

	static void NewTransformToPrimitive_GameThread(UINT PrimitiveID, const FTransform& NewTransform)
	{
		if(PrimitiveID > 0 )
		{
			auto Lambda = [PrimitiveID, NewTransform](std::shared_ptr<FScene>& SceneData)
				{
					const auto& SceneProxy = SceneData->PrimitiveSceneProxies.find(PrimitiveID);
					if(SceneProxy != SceneData->PrimitiveSceneProxies.end())
					{
						SceneProxy->second->SetSceneProxyWorldTransform(NewTransform);
					}
				};
			ENQUEUE_RENDER_COMMAND(Lambda)	
		}
		
	}

	// 게임쓰레드 호출_ 씬 렌더링 요청 함수
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
		ImGuizmo::BeginFrame();
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0,0,io.DisplaySize.x,io.DisplaySize.y);
		for(const auto& Func : ImGuizmoRenderFunctions)
		{
			Func.second();
		}

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

				for(const auto& SceneProxy : SceneData->PrimitiveSceneProxies)
				{
					SceneProxy.second->Draw();
				}
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
	// ImGUI렌더링 함수(람다) 추가 함수
	static void AddImGuiRenderFunction(const std::string& Name, const std::function<void()>& NewRenderFunction)
	{
		ImGuiRenderFunctions[Name] = NewRenderFunction;
	}
	static void AddImGuizmoRenderFunction(const std::string& Name, const std::function<void()>& NewRenderFunction)
	{
		ImGuizmoRenderFunctions[Name] = NewRenderFunction;
	}
	// 디버깅 콘솔 텍스트 추가 함수
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

	static void DrawImguizmoSelectedActor_RenderThread()
	{
		if(!CurrentSelectedActor)
		{
			return;
		}

		static ImGuizmo::OPERATION CurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE CurrentGizmoMode(ImGuizmo::WORLD);
		if(ImGui::IsKeyPressed(ImGuiKey_Q))
		{
			CurrentGizmoOperation = ImGuizmo::TRANSLATE;
		}
		if(ImGui::IsKeyPressed(ImGuiKey_W))
		{
			CurrentGizmoOperation = ImGuizmo::ROTATE;
		}
		if(ImGui::IsKeyPressed(ImGuiKey_E))
		{
			CurrentGizmoOperation = ImGuizmo::SCALE;
		}

		if(ImGui::IsKeyPressed(ImGuiKey_1))
		{
			CurrentGizmoMode = ImGuizmo::WORLD;
		}
		if(ImGui::IsKeyPressed(ImGuiKey_2))
		{
			CurrentGizmoMode = ImGuizmo::LOCAL;
		}

		const std::shared_ptr<USceneComponent>& CurrentSelectedComponent = SelectActorComponents[CurrentSelectedComponentIndex];
		FTransform ComponentTransform = CurrentSelectedComponent->GetComponentTransform();

		XMMATRIX ComponentMatrix;
		ComponentMatrix = ComponentTransform.ToMatrixWithScale();

		XMMATRIX DeltaMatrixTemp = XMMatrixIdentity();
		float* DeltaMatrix = reinterpret_cast<float*>(&DeltaMatrixTemp);

		XMMATRIX ViewMat = GEngine->Test_DeleteLater_GetViewMatrix();
		XMMATRIX ProjMat = GEngine->Test_DeleteLater_GetProjectionMatrix();

		//ProjMat = XMMatrixPerspectiveFovRH(0.5*XM_PI, 1600.0f/1200.0f, 1.0f, 1000.0f);;
		//ImGuizmo::AllowAxisFlip(true);
		ImGuizmo::Manipulate(reinterpret_cast<float*>(&ViewMat), reinterpret_cast<float*>(&ProjMat),CurrentGizmoOperation,CurrentGizmoMode,reinterpret_cast<float*>(&ComponentMatrix),DeltaMatrix);

		XMFLOAT3 DeltaTranslation;
		XMFLOAT3 DeltaRot;
		XMFLOAT3 DeltaScale;
		ImGuizmo::DecomposeMatrixToComponents(DeltaMatrix, reinterpret_cast<float*>(&DeltaTranslation),reinterpret_cast<float*>(&DeltaRot),reinterpret_cast<float*>(&DeltaScale) );
		if(CurrentGizmoOperation == ImGuizmo::TRANSLATE && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaTranslation))) > FLT_EPSILON)
		{
			const auto Lambda = [CurrentSelectedComponent, DeltaTranslation]()
				{
					CurrentSelectedComponent->AddWorldOffset(DeltaTranslation);		
				};
			ENQUEUE_IMGUIZMO_COMMAND(Lambda)
			
		}

		if ((CurrentGizmoOperation == ImGuizmo::ROTATE) && XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
		{
			if(XMVectorGetX(XMVector3Length(XMLoadFloat3(&DeltaRot))) > FLT_EPSILON)
			{
				//CurrentSelectedComponent->AddWorldRotation(DeltaRot);
				const auto Lambda = [CurrentSelectedComponent, DeltaRot]()
					{
						CurrentSelectedComponent->AddWorldRotation(DeltaRot);		
					};
				ENQUEUE_IMGUIZMO_COMMAND(Lambda)
			}

		}
	}

	// ===================================================================
public:
protected:
private:
	static void EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData)
	{
		for(const auto& NewPrimitiveProxy : SceneData->PendingAddSceneProxies)
		{
			SceneData->PrimitiveSceneProxies[NewPrimitiveProxy.first] = NewPrimitiveProxy.second;

		}
		SceneData->PendingAddSceneProxies.clear();
		SceneData->bIsFrameStart = false;
	}
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
protected:
private:
public:
protected:
private:
	static std::shared_ptr<FScene> CurrentSceneData;
};