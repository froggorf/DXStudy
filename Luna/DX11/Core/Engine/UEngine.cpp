// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEngine.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "Mesh/UStaticMesh.h"
#include "World/UWorld.h"

std::unique_ptr<UEngine> GEngine = nullptr;

UEngine::~UEngine()
{
}

void UEngine::InitEngine()
{
	CurrentWorld = std::make_shared<UWorld>();

	InitImGui();

	PostLoad();

	CurrentWorld->Init();
}

void UEngine::PostLoad()
{
	UObject::PostLoad();

	LoadAllObjectsFromFile();

	// TODO: 추후엔 Level을 직렬화를 통해 저장 및 로드를 할 수 있는 기능을 만드는게 낫지 않을까?
	// 그렇다면 여기에서 로드를 진행하기
}

void UEngine::Tick(float DeltaSeconds)
{
	if(CurrentWorld)
	{
		CurrentWorld->TickWorld(DeltaSeconds);
	}
}

void UEngine::Draw()
{
	if(CurrentWorld)
	{
		CurrentWorld->TestDrawWorld();
	}

	DrawImGui();
}

void UEngine::AddImGuiRenderFunction(const std::function<void()>& NewRenderFunction)
{
	// TODO: 중복 체크에 대한 부분이 필요
	ImGuiRenderFunctions.push_back(NewRenderFunction);
}

void UEngine::AddImGuizmoRenderFunction(const std::function<void()>& NewRenderFunction)
{
	ImGuizmoRenderFunctions.push_back(NewRenderFunction);
}


void UEngine::InitImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	
	ImGui_ImplWin32_Init(Application->GetMainWnd());
	ImGui_ImplDX11_Init(GetDevice(), GetDeviceContext());

}

void UEngine::DrawImGui()
{
	if(ImGuiRenderFunctions.size() == 0)
	{
		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	// ImGui
	for(const auto& Func : ImGuiRenderFunctions)
	{
		Func();
	}


	// ImGuizmo
	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0,0,io.DisplaySize.x,io.DisplaySize.y);
	for(const auto& Func : ImGuizmoRenderFunctions)
	{
		Func();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UEngine::LoadAllObjectsFromFile()
{
	// 디스크에 내에 있는 모든 myasset 파일을 로드하는 함수
	// TODO: 추후 디스크 읽는 라이브러리를 사용하기
	AssetManager::ReadMyAsset<UStaticMesh>("Asset/StaticMesh/SM_Racco.myasset");
	AssetManager::ReadMyAsset<UStaticMesh>("Asset/StaticMesh/SM_Cube.myasset");
	AssetManager::ReadMyAsset<UStaticMesh>("Asset/StaticMesh/SM_Sphere.myasset");

	std::cout<< "StaticMesh Size -> "<<UStaticMesh::GetStaticMeshCache().size()<<std::endl;
}
