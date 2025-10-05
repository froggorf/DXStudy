// 03.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "CoreMinimal.h"
#include "Engine/SceneView.h"
#include "Engine/Class/Light/Light.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/SceneProxy/FPrimitiveSceneProxy.h"
#include "Engine/Widget/FWidgetRenderData.h"

// 다수의 게임 쓰레드에서 단일의 렌더쓰레드가 수행할 명령을 관리하는 파이프라인
// Multi-Producer(GameThread) Single-Consumer(RenderThread) Queue

class UShapeComponent;
class UStaticMesh;
class FStaticMeshSceneProxy;
class USceneComponent;
class AActor;

inline std::atomic<bool> bIsChangeLevel = false;

struct FRenderTask
{
	UINT                                                PrimitiveID;
	std::function<void(std::shared_ptr<class FScene>&)> CommandLambda;
	FRenderTask*                                        Next;

	FRenderTask() : Next{nullptr} {}

	FRenderTask(const FRenderTask& Other) : PrimitiveID{Other.PrimitiveID}, CommandLambda{Other.CommandLambda}, Next{Other.Next} { }
};

class FRenderCommandPipe
{
	static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>>& GetRenderCommandPipe()
	{
		static Concurrency::concurrent_queue<std::shared_ptr<FRenderTask>> RenderCommandPipe;
		return RenderCommandPipe;
	}

	//static std::unique_ptr<FRenderCommandPipe> RenderCommandPipe;
	//std::atomic<Node*> Head;
	//std::atomic<Node*> Tail;

public:
	static void Enqueue(std::function<void(std::shared_ptr<class FScene>&)>& CommandLambda);

	static bool Dequeue(std::shared_ptr<FRenderTask>& Result);

private:
	FRenderCommandPipe() = default;
	~FRenderCommandPipe() = default;
};

#define ENQUEUE_RENDER_COMMAND(Lambda) \
    { \
        std::function<void(std::shared_ptr<FScene>&)> temp = (Lambda); \
        FRenderCommandPipe::Enqueue(temp); \
    }

inline std::atomic<bool> bIsGameKill               = false;
inline std::atomic<UINT> RenderingThreadFrameCount = 0;

struct FPrimitiveRenderData
{
	UINT                                  PrimitiveID = 0;
	UINT                                  MeshIndex = 0;
	std::shared_ptr<FPrimitiveSceneProxy> SceneProxy = nullptr;
	std::shared_ptr<UMaterialInterface>   MaterialInterface = nullptr;
};

struct FPostProcessRenderData
{
	FPostProcessRenderData(UINT P, const std::string& N, const std::shared_ptr<UMaterialInterface>& M, EMultiRenderTargetType RT) : Priority(P), Name(N), MaterialInterface(M), OutRenderType(RT) {}
	UINT Priority = 0;
	std::string Name = "";
	std::shared_ptr<UMaterialInterface> MaterialInterface;
	// 결과물이 나오는 멀티렌더타겟의 타입
	EMultiRenderTargetType OutRenderType;

	FPostProcessRenderData(const FPostProcessRenderData& Other);
	FPostProcessRenderData& operator=(const FPostProcessRenderData& Other);

	void SetFuncBeforeRendering(const std::vector<std::function<void()>>& NewFuncs);

	void SetSRVNames(const std::vector<std::string>& NewSRVs);
	bool operator<(const FPostProcessRenderData& Other) const;

	const std::vector<std::string>& GetSRVNames() const {return SRVNames;}
	const std::vector<std::weak_ptr<UTexture>>& GetSRVTextures() const {return SRVTextures;}
	const std::vector<std::function<void()>>& GetFuncBeforeRendering() const {return FuncBeforeRendering;}
	bool GetClearRenderTexture() const {return bClearRenderTexture;}
	bool GetClearDepthStencilTexture() const {return bClearDepthStencilTexture;}
	void SetClearRenderTexture(bool NewClear) {bClearRenderTexture = NewClear;}
	void SetClearDepthStencilTexture(bool NewClear) {bClearDepthStencilTexture = NewClear;}
private:
	std::vector<std::string> SRVNames;
	std::vector<std::weak_ptr<UTexture>> SRVTextures;
	std::vector<std::function<void()>> FuncBeforeRendering;
	bool bClearRenderTexture = true;
	bool bClearDepthStencilTexture = true;
};

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
struct FDebugRenderData
{
	float RemainTime;
	std::shared_ptr<UShapeComponent> ShapeComp;
	FTransform Transform;
	XMFLOAT4 DebugColor;
};
#endif

// 렌더링에 대한 정보를 가지고 있는 클래스 (씬 단위)
// 03.10 렌더링 쓰레드의 경우 단일 소비로 진행할 예정이므로
// 멀티쓰레드 동기화에 대한 처리 x
class FScene
{
public:
	FScene();

	virtual ~FScene()
	{
	}

	// ==================== FPrimitiveSceneProxy ====================
	// { UINT(MaterialID) , RenderData }
	std::unordered_map<UINT, std::vector<FPrimitiveRenderData>> DeferredSceneProxyRenderData;
	std::unordered_map<UINT, std::vector<FPrimitiveRenderData>> OpaqueSceneProxyRenderData;
	std::unordered_map<UINT, std::vector<FPrimitiveRenderData>> MaskedSceneProxyRenderData;
	std::unordered_map<UINT, std::vector<FPrimitiveRenderData>> TranslucentSceneProxyRenderData;

	std::set<FPostProcessRenderData> PostProcessData;
	std::shared_ptr<FPrimitiveSceneProxy> PostProcessStaticMeshSceneProxy;

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	std::vector<FDebugRenderData> DebugRenderData;
#endif

	// PrimitiveID, 
	std::map<UINT, std::vector<std::shared_ptr<FPrimitiveSceneProxy>>> PendingAddSceneProxies;
	std::vector<UINT>              PendingKillPrimitiveIDs;

	std::map<UINT, FTransform> PendingNewTransformProxies;

	// Light Info
	std::vector<FLightInfo> CurrentFrameLightInfo;
	std::shared_ptr<FStructuredBuffer> LightBuffer;

	// Decal Data
	std::vector<FDecalInfo> CurrentFrameDecalInfo;

	// UI Widget Data
	std::vector<FWidgetRenderData> CurrentFrameWidgetRenderData;
	std::shared_ptr<UMaterialInterface> M_Widget;
	std::shared_ptr<FPrimitiveSceneProxy> WidgetStaticMeshSceneProxy;

	// ==================== FPrimitiveSceneProxy ====================

	FPrimitiveRenderData DeferredMergeRenderData;

	// 0901) StaticMesh / SkeletalMesh를 위해 [2] 로 이중배열로 변경
	std::array<std::array<std::shared_ptr<UMaterialInterface>,2>,static_cast<UINT>(ELightType::Count)> M_LightShadow;

	bool bIsFrameStart;
	float LastUpdateTime=0;
	float DeltaSeconds;

	// 카메라 ViewMatrix, Projection Matrix를 관리
	FViewMatrices ViewMatrices;
	virtual const FViewMatrices& GetViewMatrices() {return ViewMatrices;}

	// 엔진 종료 시 렌더링 쓰레드를 죽이는 함수
	static void KillRenderingThread()
	{
		bIsGameKill = true;
	}

	// ImGUI 종료 함수
	static void ShutdownImgui();

	static void ClearScene_GameThread();

	// 게임쓰레드 _ 씬 데이터를 레벨의 초기화에 맞춰 변경하는 함수
#ifndef WITH_EDITOR
	static void InitSceneData_GameThread()
	{
		ENQUEUE_RENDER_COMMAND([](std::shared_ptr<FScene>& SceneData) {
			// 기존에 남아있는 렌더 명령어 모두 Dequeue
			SceneData = std::make_shared<FScene>();
			std::shared_ptr<FRenderTask> DummyTask;
			while(FRenderCommandPipe::Dequeue(DummyTask))
			{
				DummyTask->CommandLambda(SceneData);
			}
			
			
		})
	}
#endif

	// 렌더쓰레드 프레임 시작 알림 함수
	// 게임쓰레드 시작 시 호출
	static void BeginRenderFrame_GameThread(UINT GameThreadFrameCount);

	static void  BeginRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData, UINT GameThreadFrameCount);

	virtual void BeginRenderFrame();

	// 렌더 쓰레드 프레임 종료 함수 (Draw에서 호출)
	static void EndRenderFrame_GameThread();

	// 새로운 UPrimitiveComponent 생성 후 register 시 렌더링 쓰레드에게 알리는 함수
	static void AddPrimitive_GameThread(UINT PrimitiveID, std::shared_ptr<FPrimitiveSceneProxy>& SceneProxy, FTransform InitTransform);

	static void AddPrimitive_RenderThread(const std::shared_ptr<FScene>& SceneData, UINT PrimitiveID, const std::shared_ptr<FPrimitiveSceneProxy>& NewProxy);

	// 특정 프리미티브 ID의 씬 프록시를 제거해달라고 요청
	static void KillPrimitive_GameThread(UINT PrimitiveID);

	static void NewTransformToPrimitive_GameThread(UINT PrimitiveID, const FTransform& NewTransform);

	static void UpdateSkeletalMeshAnimation_GameThread(UINT PrimitiveID, const std::vector<XMMATRIX>& FinalMatrices);

	// 게임쓰레드 호출_ 씬 렌더링 요청 함수
	static void DrawScene_GameThread();

	// 특정 ID의 머테리얼의 파라미터를 바꾸는 함수
	static void SetMaterialScalarParam_GameThread(UINT PrimitiveID, UINT MeshIndex, const std::string& ParamName, float Value);

	void SetMaterialScalarParam_RenderThread(UINT PrimitiveID, UINT MeshIndex, const std::string& ParamName, float Value);

	// 특정 ID의 머테리얼의 텍스쳐 파라미터를 바꾸는 함수
	static void SetTextureParam_GameThread(UINT PrimitiveID, UINT MeshIndex, UINT TextureSlot, std::shared_ptr<UTexture> Texture);

	void SetTextureParam_RenderThread(UINT PrimitiveID, UINT MeshIndex, UINT TextureSlot, std::shared_ptr<UTexture> Texture);

	// 특정 씬 프록시의 이펙트를 Activate 시키는 함수
	static void SetNiagaraEffectActivate_GameThread(std::vector<std::shared_ptr<class FNiagaraSceneProxy>>& TargetSceneProxies, bool bNewActivate);

	void         DrawShadowMap(ELightType LightType) const;
	static void  DrawScene_RenderThread(std::shared_ptr<FScene> SceneData);
	virtual void SetDrawScenePipeline_HDR_MiddleStep();
	virtual void SetDrawScenePipeline();
	virtual void SetRSViewport();

	virtual void AfterDrawSceneAction(const std::shared_ptr<FScene> SceneData){}

	virtual XMMATRIX GetViewMatrix();
	virtual XMMATRIX GetProjectionMatrix();

	static void UpdateViewMatrix_GameThread(const FViewMatrices& NewViewMatrices);
	void UpdateViewMatrix_RenderThread(const FViewMatrices& NewViewMatrices)
	{
		ViewMatrices = NewViewMatrices;
	}

	static void SetSkyBoxTexture_GameThread(const std::string& NewEnvironmentTextureName);
	void SetSkyBoxTexture_RenderThread(const std::string& NewEnvironmentTextureName);

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	static void DrawDebugData_GameThread(const FDebugRenderData& RenderData);
	void DrawDebugData_RenderThread(const FDebugRenderData& InDebugRenderData);
#endif

	// 현재 프레임의 LightInfo 를 설정
	void SetFrameLightInfo(const std::vector<FLightInfo>& LightInfo);
	// 현재 프레임의 DecalInfo 를 설정
	void SetFrameDecalInfo(const std::vector<FDecalInfo>& DecalInfo);\
	// 현재 프레임의 WidgetRenderData를 설정
	void SetFrameWidgetRenderData(const std::vector<FWidgetRenderData>& WidgetRenderData);

	// PostProcess 추가
	static void AddPostProcess_GameThread(const FPostProcessRenderData& NewPostProcess);
	// PostProcess 삭제
	static void RemovePostProcess_GameThread(UINT Priority, const std::string& Name);

	static void SetComponentMonochrome_GameThread(UINT PrimitiveID, bool NewMonochrome);

	static void ChangeComponentVisibility_GameThread(UINT PrimitiveID, bool NewVisible);
private:
	void SetComponentMonochrome_RenderThread(UINT PrimitiveID, bool NewMonochrome);
	void ChangeComponentVisibility_RenderThread(UINT PrimitiveID, bool NewVisible);
	void AddPostProcess_RenderThread(const FPostProcessRenderData& NewPostProcess)
	{
		PostProcessData.insert(NewPostProcess);
	}
	void RemovePostProcess_RenderThread(UINT Priority, const std::string& Name);

	static void EndRenderFrame_RenderThread(std::shared_ptr<FScene>& SceneData);
};

// 엔진 초기화 시 생성되며
// RenderCommandPipe에 존재하는 Command를 계속해서 수행하는 클래스
class FRenderCommandExecutor
{
public:
	static void Execute();

	static std::shared_ptr<FScene> CurrentSceneData;
};
