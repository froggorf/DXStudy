﻿// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UWorld.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/UEditorEngine.h"
#include "Engine/UEngine.h"
#include "Engine/Class/Framework/APlayerController.h"
#include "Engine/Physics/UBoxComponent.h"
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

	if (!PersistentLevel)
	{
		MY_LOG("Error call", EDebugLogLevel::DLL_Error, "Not valid PersistentLevel");
		return;
	}

	// TODO: 0709 임시적으로 PlayerController를 해당 위치에서 생성
	PlayerController = std::dynamic_pointer_cast<APlayerController>(PersistentLevel->SpawnActor("APlayerController", {}));

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

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
void UWorld::DrawDebugBox(const XMFLOAT3& Center, const XMFLOAT3& Extent, const XMFLOAT3& LineColor, XMVECTOR Rotate, const float DebugDrawTime) const
{
	std::shared_ptr<UBoxComponent> BoxComp = std::make_shared<UBoxComponent>();
	BoxComp->SetExtent(Extent);
	BoxComp->CreateVertexBuffer();
	FDebugRenderData Data;
	Data.Transform = FTransform{Rotate, Center, {1,1,1}};
	Data.DebugColor = {LineColor.x,LineColor.y,LineColor.z, 1.0f};
	Data.RemainTime = DebugDrawTime;
	Data.ShapeComp = BoxComp;
	FScene::DrawDebugData_GameThread(Data);
}
#endif
