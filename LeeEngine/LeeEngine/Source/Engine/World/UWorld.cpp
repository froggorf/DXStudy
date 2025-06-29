// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UWorld.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/UEditorEngine.h"
#include "Engine/UEngine.h"
#include "Engine/Physics/UPhysicsEngine.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/RenderCore/RenderingThread.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

UWorld::UWorld()
{
}

UWorld::~UWorld()
{
}

void UWorld::Init()
{
	UObject::Init();

	// 월드 초기화
}

void UWorld::PostLoad()
{
	UObject::PostLoad();

	if (PersistentLevel)
	{
		PersistentLevel->PostLoad();
	}
}

void UWorld::BeginPlay()
{
	UObject::BeginPlay();

	// 테스트용 컴퓨트 셰이더 생성
	{
		std::shared_ptr<UTexture> Texture = AssetManager::CreateTexture("TestTexture", 1023, 1023, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

		//std::shared_ptr<FSetColorCS> SetColorCS = std::reinterpret_pointer_cast<FSetColorCS>(FShader::GetShader("FSetColorCS"));
		//SetColorCS->SetTargetTexture(Texture);
		//SetColorCS->SetClearColor(XMFLOAT4{0.0f,1.0f,1.0f,1.0f});
		//SetColorCS->Execute();
	}

	if (!PersistentLevel)
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "Not valid PersistentLevel");
		return;
	}
	PersistentLevel->BeginPlay();
}

void UWorld::TickWorld(float DeltaSeconds)
{
	// Level Tick
	if (PersistentLevel)
	{
		PersistentLevel->TickLevel(DeltaSeconds);
	}

	// Niagara Proxy Tick 요청
	for (auto& TickNiagaraSceneProxy : ToBeTickedNiagaraSceneProxies)
	{
		auto Lambda = [TickNiagaraSceneProxy, DeltaSeconds](std::shared_ptr<FScene>& SceneData)
		{
			TickNiagaraSceneProxy->TickCS(DeltaSeconds);
		};
		ENQUEUE_RENDER_COMMAND(Lambda);
	}
	ToBeTickedNiagaraSceneProxies.clear();
}

void UWorld::Tick()
{
}

void UWorld::SetPersistentLevel(const std::shared_ptr<ULevel>& NewLevel)
{
	gPhysicsEngine->ResetScene();
	PersistentLevel = NewLevel;
	PersistentLevel->Register();
	if (GEngine->bGameStart)
	{
		PersistentLevel->BeginPlay();
	}

#ifdef WITH_EDITOR
	// 타이틀 바 내 현재 레벨 이름 변경
	SendMessage(GEditorEngine->GetWindow(), WM_NCPAINT, true, 0);
#endif
}

void UWorld::LoadLevelInstanceByName(const std::string& NewLevelName)
{
}

void UWorld::AddLevel(const std::shared_ptr<ULevel>& NewLevel)
{
	// Set으로 설정하여 중복 추가 x
	Levels.emplace(NewLevel);
}
