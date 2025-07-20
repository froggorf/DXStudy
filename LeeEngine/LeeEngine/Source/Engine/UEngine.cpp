// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UEngine.h"

#include <windowsx.h>

#include "FAudioDevice.h"
#include "Class/Framework/UPlayerInput.h"
#include "Mesh/UStaticMesh.h"
#include "RenderCore/RenderingThread.h"
#include "World/UWorld.h"
#include "Misc/QueuedThreadPool.h"
#include "Physics/UPhysicsEngine.h"
#include "RenderCore/EditorScene.h"
#include "RenderCore/ImGUIActionTask.h"

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
	GThreadPool = std::make_unique<FQueuedThreadPool>(150);

	CreateAudioThread();

	PostLoad();

	CurrentWorld = std::make_shared<UWorld>();

	InitImGui();

	LoadDataFromDefaultEngineIni();

	CreateRenderThread();

	LoadDefaultMap();

	CurrentWorld->Init();
	Sleep(100);
#ifndef WITH_EDITOR
	// 에디터가 아닐경우 바로 게임이 실행되도록 설정
	//GameStart();
	// GameStart호출이 안되는경우가 있어 해당방식으로 수정
	bGameStart  = true;
	TimeSeconds = 0;

	GetWorld()->BeginPlay();
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

void UEngine::ChangeLevelByName(const std::string& str)
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
			gPhysicsEngine->TickPhysics(DeltaSeconds);
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

	static bool b = false;
	if(ImGui::IsKeyReleased(ImGuiKey_0))
	{
		if(!b)
		{
			b = true;
			GEngine->ChangeLevelByName("AsyncTestLevel");
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

// AI를 이용하여 생성
static EKeys VKCodeToEKey(WPARAM vk, LPARAM lParam = 0)
{
	switch (vk)
	{
		// 알파벳
	case 'A': return EKeys::A;
	case 'B': return EKeys::B;
	case 'C': return EKeys::C;
	case 'D': return EKeys::D;
	case 'E': return EKeys::E;
	case 'F': return EKeys::F;
	case 'G': return EKeys::G;
	case 'H': return EKeys::H;
	case 'I': return EKeys::I;
	case 'J': return EKeys::J;
	case 'K': return EKeys::K;
	case 'L': return EKeys::L;
	case 'M': return EKeys::M;
	case 'N': return EKeys::N;
	case 'O': return EKeys::O;
	case 'P': return EKeys::P;
	case 'Q': return EKeys::Q;
	case 'R': return EKeys::R;
	case 'S': return EKeys::S;
	case 'T': return EKeys::T;
	case 'U': return EKeys::U;
	case 'V': return EKeys::V;
	case 'W': return EKeys::W;
	case 'X': return EKeys::X;
	case 'Y': return EKeys::Y;
	case 'Z': return EKeys::Z;

		// 숫자(상단)
	case '0': return EKeys::Num0;
	case '1': return EKeys::Num1;
	case '2': return EKeys::Num2;
	case '3': return EKeys::Num3;
	case '4': return EKeys::Num4;
	case '5': return EKeys::Num5;
	case '6': return EKeys::Num6;
	case '7': return EKeys::Num7;
	case '8': return EKeys::Num8;
	case '9': return EKeys::Num9;

		// 숫자패드
	case VK_NUMPAD0: return EKeys::Numpad0;
	case VK_NUMPAD1: return EKeys::Numpad1;
	case VK_NUMPAD2: return EKeys::Numpad2;
	case VK_NUMPAD3: return EKeys::Numpad3;
	case VK_NUMPAD4: return EKeys::Numpad4;
	case VK_NUMPAD5: return EKeys::Numpad5;
	case VK_NUMPAD6: return EKeys::Numpad6;
	case VK_NUMPAD7: return EKeys::Numpad7;
	case VK_NUMPAD8: return EKeys::Numpad8;
	case VK_NUMPAD9: return EKeys::Numpad9;
	case VK_ADD:      return EKeys::NumpadAdd;
	case VK_SUBTRACT: return EKeys::NumpadSubtract;
	case VK_MULTIPLY: return EKeys::NumpadMultiply;
	case VK_DIVIDE:   return EKeys::NumpadDivide;
	case VK_DECIMAL:  return EKeys::NumpadDecimal;

		// 방향키
	case VK_UP:    return EKeys::Up;
	case VK_DOWN:  return EKeys::Down;
	case VK_LEFT:  return EKeys::Left;
	case VK_RIGHT: return EKeys::Right;

		// 마우스 버튼 (메시지에서 직접 구분하는 게 일반적이지만, VK로도 정의)
	case VK_LBUTTON: return EKeys::MouseLeft;
	case VK_RBUTTON: return EKeys::MouseRight;
	case VK_MBUTTON: return EKeys::MouseMiddle;
	case VK_XBUTTON1: return EKeys::MouseXButton1;
	case VK_XBUTTON2: return EKeys::MouseXButton2;
		// 마우스 휠은 메시지(예: WM_MOUSEWHEEL)에서 별도로 처리

		// 펑션키
	case VK_F1:  return EKeys::F1;
	case VK_F2:  return EKeys::F2;
	case VK_F3:  return EKeys::F3;
	case VK_F4:  return EKeys::F4;
	case VK_F5:  return EKeys::F5;
	case VK_F6:  return EKeys::F6;
	case VK_F7:  return EKeys::F7;
	case VK_F8:  return EKeys::F8;
	case VK_F9:  return EKeys::F9;
	case VK_F10: return EKeys::F10;
	case VK_F11: return EKeys::F11;
	case VK_F12: return EKeys::F12;

		// 특수키
	case VK_SPACE:     return EKeys::Space;
	case VK_TAB:       return EKeys::Tab;
	case VK_RETURN:    return EKeys::Enter;
	case VK_ESCAPE:    return EKeys::Escape;
	case VK_BACK:      return EKeys::Backspace;
	case VK_INSERT:    return EKeys::Insert;
	case VK_DELETE:    return EKeys::Delete;
	case VK_HOME:      return EKeys::Home;
	case VK_END:       return EKeys::End;
	case VK_PRIOR:     return EKeys::PageUp;
	case VK_NEXT:      return EKeys::PageDown;
	case VK_CAPITAL:   return EKeys::CapsLock;
	case VK_SCROLL:    return EKeys::ScrollLock;
	case VK_PAUSE:     return EKeys::PauseBreak;
	case VK_SNAPSHOT:  return EKeys::PrintScreen;
	case VK_APPS:      return EKeys::Apps;
	case VK_MENU:      // Alt (좌/우 구분 필요)
	{
		bool isRight = (lParam & 0x01000000) != 0;
		return isRight ? EKeys::RAlt : EKeys::LAlt;
	}
	case VK_LMENU:     return EKeys::LAlt;
	case VK_RMENU:     return EKeys::RAlt;
	case VK_LWIN:      return EKeys::Menu; // 윈도우키
	case VK_RWIN:      return EKeys::Menu;

		// Modifier
	case VK_SHIFT:
	{
		// 좌/우 구분 (스캔코드 사용)
		UINT scancode = (lParam & 0x00ff0000) >> 16;
		UINT vk_ex = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		if (vk_ex == VK_LSHIFT) return EKeys::LShift;
		else if (vk_ex == VK_RSHIFT) return EKeys::RShift;
		else return EKeys::LShift; // 기본값
	}
	case VK_LSHIFT: return EKeys::LShift;
	case VK_RSHIFT: return EKeys::RShift;
	case VK_CONTROL:
	{
		bool isRight = (lParam & 0x01000000) != 0;
		return isRight ? EKeys::RCtrl : EKeys::LCtrl;
	}
	case VK_LCONTROL: return EKeys::LCtrl;
	case VK_RCONTROL: return EKeys::RCtrl;

	default:
		return EKeys::None;
	}
}

void UEngine::HandleInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	static XMFLOAT2 LastMousePosition;
	static XMFLOAT2 LastMouseDelta;

	FInputEvent InputEvent;
	InputEvent.Key = EKeys::None;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		InputEvent.Key= EKeys::MouseLeft;
		InputEvent.CurPosition.x = GET_X_LPARAM(lParam);
		InputEvent.CurPosition.y = GET_Y_LPARAM(lParam);
		InputEvent.bKeyDown = (msg == WM_LBUTTONDOWN);
		break;
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		InputEvent.Key= EKeys::MouseRight;
		InputEvent.CurPosition.x = GET_X_LPARAM(lParam);
		InputEvent.CurPosition.y = GET_Y_LPARAM(lParam);
		InputEvent.bKeyDown = (msg == WM_RBUTTONDOWN);
		break;
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			LastMouseDelta = {x - LastMousePosition.x, y - LastMousePosition.y};
			LastMousePosition = {static_cast<float>(x),static_cast<float>(y)};

			InputEvent.Key = EKeys::MouseXY2DAxis;
			InputEvent.bKeyDown = false;
			InputEvent.Delta = LastMouseDelta;
			InputEvent.CurPosition = LastMousePosition;
		}
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			InputEvent.Key = EKeys::MouseWheelUp;	
		}
		else
		{
			InputEvent.Key = EKeys::MouseWheelDown;	
		}
		InputEvent.bKeyDown = true;
		
		
	break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		InputEvent.Key = VKCodeToEKey(wParam,lParam);
		InputEvent.bKeyDown = (msg == WM_KEYDOWN);
		InputEvent.CurPosition = LastMousePosition;
		InputEvent.Delta = LastMouseDelta;
		break;
	default:
		// 잘못된 데이터는 그냥 종료
		return;
	}
	

	/// UI에서 처리를 해보고
	
	/// 남은것에 대해서 PlayerInput으로 처리
	if (GetWorld() && GetWorld()->GetPlayerController())
	{
		APlayerController* PC = GetWorld()->GetPlayerController();
		PC->PlayerInput->LastMousePosition = LastMousePosition;
		PC->PlayerInput->LastMouseDelta = LastMouseDelta;
		PC->PlayerInput->HandleInput(InputEvent);
	}
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
			if(AssetTypeFromName == "M_" || AssetTypeFromName == "T_" || AssetTypeFromName == "NS" || AssetTypeFromName == "MI" || AssetTypeFromName == "CV" || AssetTypeFromName == "SB")
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
