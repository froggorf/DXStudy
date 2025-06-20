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
	GetCurrentDirectory(100, test);

	// wstring -> string
	std::wstring temp = test;
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), (int)temp.size(), NULL, 0, NULL, NULL);
	CurrentDirectory.resize(size_needed);
	WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), (int)temp.size(), &CurrentDirectory[0], size_needed, NULL, NULL);

	while (true)
	{
		auto p = CurrentDirectory.find("\\");
		if (p == std::string::npos)
		{
			break;
		}
		CurrentDirectory.replace(p, 1, "/");
	}
	EngineDirectory = CurrentDirectory;
	if (auto p = EngineDirectory.find("MyGame"))
	{
		EngineDirectory.replace(p, p + 5, "LeeEngine");
	}

	// GameThread, RenderThread 제외한 코어 수
	GThreadPool = std::make_unique<FQueuedThreadPool>(50);

	CreateAudioThread();

	PostLoad();

	CurrentWorld = std::make_shared<UWorld>();

	InitImGui();

	LoadDataFromDefaultEngineIni();

	CreateRenderThread();

	LoadDefaultMap();

	CurrentWorld->Init();

	//RenderThread = std::thread(&UEngine::Draw,this);

#ifndef WITH_EDITOR
	// 에디터가 아닐경우 바로 게임이 실행되도록 설정
	GameStart();
#endif
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
	std::string   DataName;
	while (DataFile >> DataName)
	{
		std::string Data;
		DataFile >> Data; // =
		DataFile >> Data;
		EngineData[DataName] = Data;
	}
}

void UEngine::LoadDefaultMap()
{
	AssetManager::GetAsyncAssetCache(GetDefaultMapName(), [this](std::shared_ptr<UObject> DefaultLevel)
	{
		// 레벨 정보를 생성자를 통해서 인스턴스를 만드는 방식
		std::shared_ptr<ULevel> DefaultLevelInstance = std::make_shared<ULevel>(std::dynamic_pointer_cast<ULevel>(DefaultLevel).get());
		CurrentWorld->SetPersistentLevel(DefaultLevelInstance);
	});
}

const std::string& UEngine::GetDefaultMapName()
{
	return EngineData["GameDefaultMap"];
}

void UEngine::DELETELATER_TestChangeLevel(const std::string& str)
{
	// 비동기 로드 진행
	AssetManager::GetAsyncAssetCache(str, [this](std::shared_ptr<UObject> LevelObject)
	{
		if(LevelObject)
		{
			// 레벨 정보를 생성자를 통해서 인스턴스를 만드는 방식
			std::shared_ptr<ULevel> TTT = std::make_shared<ULevel>(std::dynamic_pointer_cast<ULevel>(LevelObject).get());

			CurrentWorld->SetPersistentLevel(TTT);		
		}
		
	});
}

void UEngine::GameStart()
{
	// 게임 시작
	if (bGameStart)
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "Already start game");
		return;
	}
	if (!GetWorld() || !GetWorld()->GetPersistentLevel())
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "No valid PersistentLevel");
		return;
	}

	bGameStart  = true;
	TimeSeconds = 0;

	GetWorld()->BeginPlay();
}

void UEngine::Tick(float DeltaSeconds)
{
	this->DeltaSeconds = DeltaSeconds;
#ifdef WITH_EDITOR
	std::shared_ptr<FImGUITask> Task;
	while (FImGuizmoCommandPipe::Dequeue(Task))
	{
		Task->CommandLambda();
	}
#endif
	if (GAudioDevice)
	{
		GAudioDevice->GameThread_AudioUpdate();
	}

	++GameThreadFrameCount;

	FScene::BeginRenderFrame_GameThread(GameThreadFrameCount);

	if (bGameStart)
	{
		TimeSeconds += DeltaSeconds;
		if (CurrentWorld)
		{
			CurrentWorld->TickWorld(DeltaSeconds);
		}
	}

	for (const auto& IDAndComponent : ComponentsTransformDirty)
	{
		UINT PrimitiveID = IDAndComponent.first;
		if (IDAndComponent.second && IDAndComponent.second->IsPrimitive())
		{
			FScene::NewTransformToPrimitive_GameThread(PrimitiveID, IDAndComponent.second->GetComponentTransform());
		}
	}
	if (!ComponentsTransformDirty.empty())
	{
		ComponentsTransformDirty.clear();	
	}
	

	if (GameThreadFrameCount > RenderingThreadFrameCount + 3)
	{
		FScene::EndRenderFrame_GameThread();
	}
	else
	{
		FScene::DrawScene_GameThread();
	}

	// TODO: DeleteLater 임시레벨 변경 코드
	//static int i = 0;
	//++i;
	//if(i == 10000)
	//{
	//	GEngine->DELETELATER_TestChangeLevel("Test2Level");
	//}
	//if(i == 20000)
	//{
	//	i = 0;
	//	GEngine->DELETELATER_TestChangeLevel("TestLevel");
	//}
	static bool b = false;
	if(ImGui::IsKeyReleased(ImGuiKey_0))
	{
		if(!b)
		{
			b = true;
			GEngine->DELETELATER_TestChangeLevel("AsyncTestLevel");
		}
		
	}
	if (ImGui::IsKeyReleased(ImGuiKey_9))
	{
		b=false;
	}

}

void UEngine::MakeComponentTransformDirty(std::shared_ptr<USceneComponent>& SceneComponent)
{
	if (!SceneComponent->IsPrimitive())
	{
		return;
	}
	UINT PrimitiveID = SceneComponent->GetPrimitiveID();
	if (SceneComponent && PrimitiveID > 0)
	{
		ComponentsTransformDirty[PrimitiveID] = SceneComponent;
	}
}

void UEngine::JoinThreadsAtDestroy()
{
	if (RenderThread.joinable())
	{
		RenderThread.join();
	}

	GAudioDevice->GameKill();
	if (AudioThread.joinable())
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
	RenderThread = std::thread(&FRenderCommandExecutor::Execute);
}

void UEngine::CreateAudioThread()
{
	GAudioDevice = std::make_shared<FAudioDevice>();
	AudioThread  = std::thread(&FAudioThread::Execute);
}

void MyCreateWindow(ImGuiViewport* Viewport)
{
	std::mutex              mtx;
	std::condition_variable cv;
	bool                    done = false;

	ENQUEUE_IMGUI_COMMAND([&]() { ImGui_ImplWin32_CreateWindow(Viewport); { std::lock_guard<std::mutex> lock(mtx); done = true; } cv. notify_one(); });

	// 메인쓰레드에서 완료될 때까지 대기
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock,
			[&]()
			{
				return done;
			});
}

void MyShowWindow(ImGuiViewport* Viewport)
{
	std::mutex              mtx;
	std::condition_variable cv;
	bool                    done = false;

	ENQUEUE_IMGUI_COMMAND([&]() { ImGui_ImplWin32_ShowWindow(Viewport); { std::lock_guard<std::mutex> lock(mtx); done = true; } cv. notify_one(); });

	// 메인쓰레드에서 완료될 때까지 대기
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock,
			[&]()
			{
				return done;
			});
}

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Application 인스턴스 접근 (싱글턴, 전역 포인터 등)
	if (GEngine)
	{
		if (GEngine->GetApplication())
		{
			return GEngine->GetApplication()->MsgProc(hWnd, msg, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void UEngine::InitImGui()
{
	// Imgui 의 렌더링과 업데이트 처리를 렌더링 쓰레드에서 진행하는데
	// 메인쓰레드가 아닌 곳에서 윈도우가 생성되면 메시지 프록이 정상적으로 적용되지 않는 현상이 발생하여
	// Imgui의 메시지 처리가 진행되지 않아 프리징 난듯이 일어난다.
	// 따라서
	/*
	ImGuiPlatformIO& PlatformIO = ImGui::GetPlatformIO();
	PlatformIO.Platform_CreateWindow = MyCreateWindow;
	PlatformIO.Platform_ShowWindow = MyShowWindow;
	 */
	//를 추가해줘서 렌더링쓰레드가 아니라 게임쓰레드에서 윈도우를 생성하게 하고,
	// 또한 아래의 윈도우 정보를 미리 만들어놓고,
	// lpszClassName 이 Imgui 내부에서 만드는것과 동일하면 미리 만든 윈도우를 사용한다고함
	// 따라서 내 윈도우 메시지 프록이 그대로 전달돼서 진행되는것
	WNDCLASSEXW wc   = {};
	wc.cbSize        = sizeof(WNDCLASSEXW);
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = MyWndProc; // <- 네 메시지 처리 함수
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = ::GetModuleHandle(nullptr);
	wc.hIcon         = nullptr;
	wc.hCursor       = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = L"ImGui Platform"; // ImGui가 사용하는 클래스명과 같게!
	wc.hIconSm       = nullptr;
	RegisterClassExW(&wc);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends

	ImGui_ImplWin32_Init(Application->GetMainWnd());
	ImGui_ImplDX11_Init(GDirectXDevice->GetDevice().Get(), GDirectXDevice->GetDeviceContext().Get());

	MY_LOG("Init", EDebugLogLevel::DLL_Display, "Imgui init success");

	// 폰트 로드
	std::string  FontPath = EngineDirectory + "/Content/Editor/Font/Roboto.ttf";
	float        FontSize = 18.0f;
	ImFontConfig FontConfig;
	FontConfig.OversampleH = 3;
	FontConfig.OversampleV = 3;
	FontConfig.PixelSnapH  = true;
	io.Fonts->AddFontFromFileTTF(FontPath.c_str(), FontSize, &FontConfig, io.Fonts->GetGlyphRangesKorean());
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	ImGuiPlatformIO& PlatformIO      = ImGui::GetPlatformIO();
	PlatformIO.Platform_CreateWindow = MyCreateWindow;
}

void UEngine::LoadAllObjectsFromFile()
{
	// 디스크에 내에 있는 모든 myasset 파일을 로드하는 함수
	std::vector<std::filesystem::path> MyAssetFiles;
	MyAssetFiles.reserve(100);
	std::string ContentDirectory = CurrentDirectory + "/Content";
	for (const auto& Entry : std::filesystem::recursive_directory_iterator(ContentDirectory))
	{
		if (Entry.is_regular_file() && Entry.path().extension() == ".myasset")
		{
			std::string FileName = Entry.path().stem().string();
			std::string FullPath = Entry.path().generic_string();
			// {에셋이름 - 에셋경로} 맵 추가
			AssetManager::GetAssetNameAndAssetPathMap()[FileName] = FullPath;
			// 머테리얼과 텍스쳐는 프리로드 되도록 변경
			// 06.20 애니메이션 에셋을 위해 스켈레탈 메시 정보도 미리 로드되도록 변경
			std::string AssetTypeFromName = FileName.substr(0,2);
			if(AssetTypeFromName == "M_" || AssetTypeFromName == "T_")
			{
				MyAssetFiles.push_back(Entry.path());	
			}
		}
	}

	for (const auto& File : MyAssetFiles)
	{
		std::string FilePath = File.generic_string();

		AssetManager::ReadMyAsset(FilePath);
	}

	//MY_LOG("Load", EDebugLogLevel::DLL_Warning, "Load All Objects From File Success");
}
