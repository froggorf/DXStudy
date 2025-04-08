// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UEngine.h"

#include "FAudioDevice.h"
#include "Mesh/UStaticMesh.h"
#include "RenderCore/RenderingThread.h"
#include "World/UWorld.h"
#include "Mesh/USkeletalMesh.h"
#include "Misc/QueuedThreadPool.h"
#include "RenderCore/EditorScene.h"


std::shared_ptr<UEngine> GEngine = nullptr;

UEngine::~UEngine()
{
	
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
	EngineDirectory = CurrentDirectory;
	if(auto p = EngineDirectory.find("MyGame"))
	{
		EngineDirectory.replace(p, p+5, "LeeEngine");
	}

	GThreadPool = std::make_unique<FQueuedThreadPool>(std::thread::hardware_concurrency()-2); // GameThread, RenderThread
	
	PostLoad();


	CurrentWorld = std::make_shared<UWorld>();

	InitImGui();

	LoadDataFromDefaultEngineIni();


	LoadDefaultMap();

	CurrentWorld->Init();

	//RenderThread = std::thread(&UEngine::Draw,this);
	CreateRenderThread();

	CreateAudioThread();

	
	
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

void UEngine::GameStart()
{
	// 게임 시작
	if(bGameStart)
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "Already start game");
		return;
	}
	if(!GetWorld() || !GetWorld()->GetPersistentLevel())
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "No valid PersistentLevel");
		return;
	}

	bGameStart= true;
	TimeSeconds = 0;

	GetWorld()->BeginPlay();
}

void UEngine::Tick(float DeltaSeconds)
{
	this->DeltaSeconds = DeltaSeconds;
#ifdef WITH_EDITOR
	std::shared_ptr<FImGUITask> Task;
	while(FImGuizmoCommandPipe::Dequeue(Task))
	{
		Task->CommandLambda();
	}
#endif

	++GameThreadFrameCount;


	FScene::BeginRenderFrame_GameThread(GameThreadFrameCount);

	if(bGameStart)
	{
		TimeSeconds += DeltaSeconds;
		if(CurrentWorld)
		{
			CurrentWorld->TickWorld(DeltaSeconds);
		}	
	}
	


	for(const auto& IDAndComponent : ComponentsTransformDirty)
	{
		UINT PrimitiveID = IDAndComponent.first;
		if(IDAndComponent.second && IDAndComponent.second->IsPrimitive())
		{
			FScene::NewTransformToPrimitive_GameThread(PrimitiveID, IDAndComponent.second->GetComponentTransform());	
		}
	}
	ComponentsTransformDirty.clear();

	if(GameThreadFrameCount > RenderingThreadFrameCount + 3)
	{
		FScene::EndRenderFrame_GameThread();
	}else
	{
		FScene::DrawScene_GameThread();	
	}
	
}

void UEngine::MakeComponentTransformDirty(std::shared_ptr<USceneComponent>& SceneComponent)
{
	if(!SceneComponent->IsPrimitive())
	{
		return;
	}
	UINT PrimitiveID = SceneComponent->GetPrimitiveID() ;
	if(SceneComponent && PrimitiveID> 0)
	{
		ComponentsTransformDirty[PrimitiveID] = SceneComponent;
	}
}

void UEngine::JoinThreadsAtDestroy()
{
	if(RenderThread.joinable())
	{
		RenderThread.join();
	}

	GAudioDevice->GameKill();
	if(AudioThread.joinable())
	{
		AudioThread.join();
	}
	
}


void UEngine::HandleInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 엔진에서의 인풋 관리
}

void UEngine::CreateRenderThread()
{
	RenderThread = std::thread(&FRenderCommandExecutor::Execute, std::make_shared<FScene>());
}

void UEngine::CreateAudioThread()
{
	GAudioDevice = std::make_shared<FAudioDevice>();
	AudioThread = std::thread(&FAudioDevice::AudioThread_Update, GAudioDevice);
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

	// 폰트 로드
	std::string FontPath = EngineDirectory+"/Content/Editor/Font/Roboto.ttf";
	float FontSize = 18.0f;
	ImFontConfig FontConfig;
	FontConfig.OversampleH= 3;
	FontConfig.OversampleV= 3;
	FontConfig.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF(FontPath.c_str(),FontSize, &FontConfig, io.Fonts->GetGlyphRangesKorean());
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

}

void UEngine::LoadAllObjectsFromFile()
{
	// 디스크에 내에 있는 모든 myasset 파일을 로드하는 함수
	std::vector<std::filesystem::path> MyAssetFiles;
	MyAssetFiles.reserve(100);
	std::string ContentDirectory = CurrentDirectory + "/Content";
	for(const auto& Entry : std::filesystem::recursive_directory_iterator(ContentDirectory))
	{
		if(Entry.is_regular_file()&&Entry.path().extension() == ".myasset")
		{
			MyAssetFiles.push_back(Entry.path());
		}
	}

	for(const auto& File : MyAssetFiles)
	{
		std::string FilePath = File.string();
		while(true)
		{
			auto p = FilePath.find("\\");
			if (p == std::string::npos)
			{
				break;
			}
			FilePath.replace(p,1, "/");	
		}

		
		AssetManager::ReadMyAsset(FilePath);
	}


	MY_LOG("Load",EDebugLogLevel::DLL_Warning, "Load All Objects From File Success");
}
