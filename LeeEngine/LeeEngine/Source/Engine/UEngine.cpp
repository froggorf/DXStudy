// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UEngine.h"

#include <ImGuizmo.h>

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "Mesh/UStaticMesh.h"
#include "RenderCore/RenderingThread.h"
#include "World/UWorld.h"

#include <threads.h>

std::shared_ptr<UEngine> GEngine = nullptr;

UEngine::~UEngine()
{
	FScene::KillRenderingThread();
	if(RenderThread.joinable())
	{
		RenderThread.join();	
	}
	
}

void UEngine::InitEngine()
{
	TCHAR test[100];
	GetCurrentDirectory(100,test);
	std::wstring temp = test;
	CurrentDirectory = std::string(temp.begin(),temp.end());
	while(true)
	{
		auto p = CurrentDirectory.find("\\");
		if (p == std::string::npos)
		{
			break;
		}
		CurrentDirectory.replace(p,1, "/");
	}

	PostLoad();


	CurrentWorld = std::make_shared<UWorld>();

	InitImGui();

	LoadDataFromDefaultEngineIni();


	LoadDefaultMap();

	CurrentWorld->Init();

	//RenderThread = std::thread(&UEngine::Draw,this);
	RenderThread = std::thread(&FRenderCommandExecutor::Execute);
}

void UEngine::PostLoad()
{
	UObject::PostLoad();

	LoadAllObjectsFromFile();

	GDirectXDevice->BuildAllShaders();
	
	// TODO: 추후엔 Level을 직렬화를 통해 저장 및 로드를 할 수 있는 기능을 만드는게 낫지 않을까?
	// 그렇다면 여기에서 로드를 진행하기


}

void UEngine::LoadDataFromDefaultEngineIni()
{
	// DefaultEngine.ini
	std::string FinalFilePath = GetDirectoryPath() + "/Config/DefaultEngine.ini";

	std::ifstream DataFile(FinalFilePath.data());
	std::string DataName;
	while (DataFile >> DataName)
	{
		std::string Data;
		DataFile >> Data;	// =
		DataFile >> Data;
		EngineData[DataName] = Data;
	}
}

void UEngine::LoadDefaultMap()
{
	const ULevel* DefaultLevelInstance = ULevel::GetLevelInstanceByName(GetDefaultMapName());
	std::shared_ptr<ULevel> LoadLevel = std::make_shared<ULevel>(DefaultLevelInstance);
	CurrentWorld->SetPersistentLevel(LoadLevel);
}

const std::string& UEngine::GetDefaultMapName()
{
	return EngineData["GameDefaultMap"];
}

void UEngine::Tick(float DeltaSeconds)
{
	std::cout <<  ++GameThreadFrameCount << std::endl;
	FScene::BeginRenderFrame_GameThread();
	if(CurrentWorld)
	{
		CurrentWorld->TickWorld(DeltaSeconds);
	}


	FScene::DrawScene_GameThread();

	//Draw();

	
	//DrawImGui();

	

}

void UEngine::Draw()
{
	
		//GDirectXDevice->GetDeviceContext()->OMSetRenderTargets(1, GDirectXDevice->GetRenderTargetView().GetAddressOf(),  GDirectXDevice->GetDepthStencilView().Get());
		//GDirectXDevice->GetDeviceContext()->RSSetViewports(1, GDirectXDevice->GetScreenViewport());

		//const float clearColor[] = {0.2f, 0.2f, 0.2f,1.0f};
		//GDirectXDevice->GetDeviceContext()->ClearRenderTargetView( GDirectXDevice->GetRenderTargetView().Get(), clearColor);
		//GDirectXDevice->GetDeviceContext()->ClearDepthStencilView( GDirectXDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		//GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
		//{
		//	// 셰이더 설정
		//	GDirectXDevice->GetDeviceContext()->VSSetShader(GDirectXDevice->GetStaticMeshVertexShader().Get(), nullptr, 0);
		//	GDirectXDevice->GetDeviceContext()->PSSetShader(GDirectXDevice->GetTexturePixelShader().Get(), nullptr, 0);
		//	// 상수버퍼 설정
		//	GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
		//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(0,1,GDirectXDevice->GetFrameConstantBuffer().GetAddressOf());
		//	GDirectXDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
		//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(1,1, GDirectXDevice->GetObjConstantBuffer().GetAddressOf());
		//	GDirectXDevice->GetDeviceContext()->PSSetConstantBuffers(2,1, GDirectXDevice->GetLightConstantBuffer().GetAddressOf());


		//	// 인풋 레이아웃
		//	GDirectXDevice->GetDeviceContext()->IASetInputLayout(GDirectXDevice->GetInputLayout().Get());
		//	GDirectXDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//	// Frame 상수 버퍼 설정
		//	{
		//		// 뷰, 프로젝션 행렬
		//		{
		//			FrameConstantBuffer fcb;
		//			fcb.View = XMMatrixTranspose(Test_DeleteLater_GetViewMatrix());
		//			fcb.Projection = XMMatrixTranspose(Test_DeleteLater_GetProjectionMatrix());
		//			fcb.LightView = XMMatrixTranspose(XMMatrixIdentity());//m_LightView
		//			fcb.LightProj = XMMatrixTranspose(XMMatrixIdentity()); //m_LightProj
		//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetFrameConstantBuffer().Get(), 0, nullptr, &fcb, 0, 0);

		//		}

		//		// 라이팅 관련
		//		{
		//			LightFrameConstantBuffer lfcb;
		//			DirectionalLight TempDirectionalLight;
		//			TempDirectionalLight.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		//			TempDirectionalLight.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		//			TempDirectionalLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		//			XMStoreFloat3(&TempDirectionalLight.Direction, XMVector3Normalize(XMVectorSet(0.57735f, -0.57735f, 0.57735f,0.0f)));
		//			lfcb.gDirLight = TempDirectionalLight;

		//			PointLight TempPointLight;
		//			TempPointLight.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		//			TempPointLight.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		//			TempPointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		//			TempPointLight.Att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
		//			//m_PointLight.Position = XMFLOAT3(796.5f, 700.0f, 0.7549f);
		//			TempPointLight.Position = XMFLOAT3(0.0f,-2.5f,0.0f);
		//			TempPointLight.Range    = 0.0f;
		//			lfcb.gPointLight = TempPointLight;
		//			// Convert Spherical to Cartesian coordinates.
		//			lfcb.gEyePosW = XMFLOAT3{GEngine->Test_DeleteLater_GetCameraPosition()};
		//			GDirectXDevice->GetDeviceContext()->UpdateSubresource(GDirectXDevice->GetLightConstantBuffer().Get(),0,nullptr,&lfcb, 0,0);
		//		}
		//	}




		//	// Sampler State 설정
		//	GDirectXDevice->GetDeviceContext()->PSSetSamplers(0, 1, GDirectXDevice->GetSamplerState().GetAddressOf());

		//	if(CurrentWorld)
		//	{
		//		CurrentWorld->TestDrawWorld();
		//	}
		//	//DrawImGui();


		//}
		//HR(GDirectXDevice->GetSwapChain()->Present(0, 0));
	
	
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
	ImGui_ImplDX11_Init(GDirectXDevice->GetDevice().Get(), GDirectXDevice->GetDeviceContext().Get());

	MY_LOG("Init",EDebugLogLevel::DLL_Display, "Imgui init success");
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

	/*
	 *디스크를 읽는것 자체는 문제가 없으나,
	 *지금 그 게 Level인지, StaticMesh인지 그걸 모르잖아.
	 *그러니까 ...
	 *파일들 첫글자에
	 */

	// Mesh
	AssetManager::ReadMyAsset(CurrentDirectory + "/Content/StaticMesh/SM_Racco.myasset");
	AssetManager::ReadMyAsset(CurrentDirectory+"/Content/StaticMesh/SM_Cube.myasset");
	AssetManager::ReadMyAsset(CurrentDirectory+"/Content/StaticMesh/SM_Sphere.myasset");

	// Level
	AssetManager::ReadMyAsset(CurrentDirectory+"/Content/Level/TestLevel.myasset");


	MY_LOG("Load",EDebugLogLevel::DLL_Warning, "Load All Objects From File Success");
}
