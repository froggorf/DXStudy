// 05.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "Engine/Material/UMaterial.h"
#include "Engine/Mesh/UStaticMesh.h"

#define MaxParticleCount 500
#define MaxRibbonPointCount 2000
#define ParticleDataRegister 20

// 파티클 스폰 구조체
struct FParticleSpawn
{
	UINT  SpawnCount;
	float arrPaddding[3];
};

// 파티클 구조체
struct FParticleData
{
	XMFLOAT3 LocalPos{};       // 소유 오브젝트로 부터의 상대 좌표
	XMFLOAT3 WorldPos{};       // 파티클의 월드 좌표
	XMFLOAT3 WorldRotation{};  // 파티클의 월드 로테이션
	XMFLOAT3 WorldInitScale{}; // 파티클 생성 시 초기 크기
	XMFLOAT3 WorldScale{};     // 파티클 월드 크기
	XMFLOAT4 Color{};          // 파티클 색상

	XMFLOAT3 Force{};    // 파티클에 주어지고 있는 힘의 총합
	XMFLOAT3 Velocity{}; // 파티클 속도
	float    Mass{};     // 파티클 질량

	float Age{};           // 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float Life{};          // 파티클 최대 수명
	float NormalizedAge{}; // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int UCount{}; // 파티클의 텍스쳐 UV
	int VCount{};

	int Active; // 파티클 활성화 여부
};

// 파티클에 적용될 각각의 기능들
enum class EParticleModule
{
	PM_Spawn,
	PM_SpawnBurst,
	PM_AddVelocity,
	PM_Scale,
	PM_AddTickVelocity,
	PM_Render,
	PM_UVAnim,
	PM_AddRotation,

	PM_END,
};

// Particle Module
struct FParticleModule
{
	// Activate State
	int ActivateState{}; // 파티클 전체에 대한 활성 상태 (0 : Activate, 1 : Deactivate, 2 : Reset)

	// Spawn Modlue
	float    SpawnRate{};  // 초당 파티클 생성량
	XMFLOAT4 StartColor{}; // 초기 파티클 색상
	int      bIsLoop{};

	XMFLOAT3 MinScale{};    // 생성 시 최소 크기
	XMFLOAT3 MaxScale{};    // 생성 시 최대 크기
	XMFLOAT3 MinRotation{}; // 생성 시 최소 회전
	XMFLOAT3 MaxRotation{}; // 생성 시 최대 회전
	float    MinLife{};     // 생성 시 최소 주기
	float    MaxLife{};     // 생성 시 최대 주기
	int      SpawnShape{};  // 0 : Box, 1 : Sphere
	XMFLOAT3 SpawnShapeScale{};
	int      SpaceType{}; // 0 : Local, 1 : World

	UINT     BlockSpawnShape{};      // 0 : Box,  1: Sphere
	XMFLOAT3 BlockSpawnShapeScale{}; // SpawnShapeScale.x == Radius

	// Spawn Burst
	UINT  SpawnBurstCount{}; // 한번에 발생시키는 Particle 수
	UINT  SpawnBurstRepeat{};
	float SpawnBurstRepeatTime{};
	float AccSpawnBurstRepeatTime{};

	// Add Velocity
	UINT     AddVelocityType{}; // 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	XMFLOAT3 AddVelocityFixedDir{};
	XMFLOAT3 AddMinSpeed{};
	XMFLOAT3 AddMaxSpeed{};

	// Scale Module
	float StartScale{};
	float EndScale{};

	// Add Tick Velocity 모듈
	XMFLOAT3 AddTickVelocity;

	// Render Module
	XMFLOAT4 EndColor{};          // 파티클 최종 색상
	int      FadeOut{};           // 0 : Off, 1 : Normalized Age
	float    StartRatio{};        // FadeOut 효과가 시작되는 Normalized Age 지점
	UINT     VelocityAlignment{}; // 속도 정렬 0 : Off, 1 : On
	UINT     CrossMesh{};         // 십자형태 메쉬 사용 0 : Off, 1 : ON

	// UVAnim 모듈
	int UCount{}; // 스프라이트 텍스쳐의 UV 갯수
	int VCount{};

	// AddRotation 모듈
	XMFLOAT3 AddRotation;

	// 추가 데이터
	XMFLOAT3 ObjectWorldPos{};

	// Module On / Off
	int Module[static_cast<UINT>(EParticleModule::PM_END)] = {0,};

	void LoadDataFromFile(const nlohmann::basic_json<>& Data);
};

// 모든 이펙트 렌더링의 기본이 되는 클래스
class FNiagaraRendererProperty
{
public:
	FNiagaraRendererProperty()          = default;
	virtual ~FNiagaraRendererProperty() = default;

	virtual void Render();

	// Material Interface
	std::shared_ptr<UMaterialInterface> GetMaterialInterface() const
	{
		return MaterialInterface;
	}

	void SetMaterialInterface(const std::shared_ptr<UMaterialInterface>& NewMaterialInterface)
	{
		MaterialInterface = NewMaterialInterface;
	}

	// Texture
	const std::vector<std::shared_ptr<UTexture>>& GetTextureData() const
	{
		return OverrideTextures;
	}

	virtual void SetParticleTextures(const nlohmann::basic_json<>& Data);

	// Load
	virtual void LoadDataFromFile(const nlohmann::basic_json<>& Data);

protected:
	std::vector<std::shared_ptr<UTexture>> OverrideTextures;
	std::shared_ptr<UMaterialInterface>    MaterialInterface;
};

// 빌보드 스프라이트를 렌더링 하기 위한 데이터가 들어있는 클래스
class FNiagaraRendererBillboardSprites : public FNiagaraRendererProperty
{
public:
	FNiagaraRendererBillboardSprites()
	{
		MaterialInterface = UMaterial::GetMaterialCache("MI_NiagaraBillboardSprite");
		StaticMesh        = UStaticMesh::GetStaticMesh("SM_Point");
	}

	~FNiagaraRendererBillboardSprites() override = default;

	void Render() override;

protected:
	std::shared_ptr<UStaticMesh> StaticMesh;
};

// 일반 스프라이트를 렌더링 하기 위한 데이터가 들어있는 클래스
// 빌보드 클래스에서 머테리얼만 다른 클래스,
class FNiagaraRendererSprites : public FNiagaraRendererBillboardSprites
{
public:
	FNiagaraRendererSprites()
	{
		MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraSprite");
		StaticMesh        = UStaticMesh::GetStaticMesh("SM_Point");
	}

	~FNiagaraRendererSprites() override = default;
};

// 스태틱 메쉬를 이펙트의 요소로 렌더링 하는 클래스
class FNiagaraRendererMeshes : public FNiagaraRendererProperty
{
public:
	FNiagaraRendererMeshes()
	{
		BaseStaticMesh    = UStaticMesh::GetStaticMesh("SM_Cube");
		MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraMesh");
	}

	~FNiagaraRendererMeshes() override = default;

	void Render() override;

	virtual void SetStaticMesh(const std::shared_ptr<UStaticMesh> InStaticMesh)
	{
		BaseStaticMesh = InStaticMesh;
	}

	void LoadDataFromFile(const nlohmann::basic_json<>& Data) override;

protected:
	std::shared_ptr<UStaticMesh> BaseStaticMesh;
};

// 리본을 이펙트의 요소로 렌더링 하는데 필요한 데이터를 관리하는 클래스
class FNiagaraRendererRibbons : public FNiagaraRendererProperty
{
	friend class FNiagaraRibbonEmitter;

public:
	FNiagaraRendererRibbons()
	{
		MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraRibbon");
	}

	void Render() override;
};

class FNiagaraEmitter
{
public:
	FNiagaraEmitter();
	virtual ~FNiagaraEmitter() = default;

	virtual void Tick(float DeltaSeconds, const FTransform& SceneTransform);
	virtual void Render() const;

	virtual std::shared_ptr<FNiagaraEmitter> GetEmitterInstance() const;

	// 매 프레임 생성되는 파티클 수를 계산하는 함수
	void CalcSpawnCount(float DeltaSeconds);

	// 파티클을 리셋하여 다시 처음부터 실행되게 하도록 하는 함수
	void Reset()
	{
		bFirstTick = true;
	}

	virtual void LoadDataFromFile(const nlohmann::basic_json<>& Data);

public:
	// 파티클을 렌더링 하기 위한 데이터가 들은 변수
	// 머테리얼, 메쉬 등
	std::shared_ptr<FNiagaraRendererProperty> RenderProperty;
	FParticleModule                           Module;

	// 리본 렌더러에서는 Deactivated 된 상태에서도 Tick만 안될 뿐, 렌더링은 되어야한다.
	// 그렇기에 해당 변수를 통해 제어
	bool bUpdateAndRenderAtDeactivate = false;

protected:
	// 파티클 활성화 시간
	float AccTime;
	bool  bFirstTick = true;

	// 파티클의 업데이트를 담당하는 ComputeShader & 구조화버퍼들
	static std::shared_ptr<FTickParticleCS> TickParticleCS;
	std::shared_ptr<FStructuredBuffer>      ParticleBuffer;
	std::shared_ptr<FStructuredBuffer>      SpawnBuffer;
	std::shared_ptr<FStructuredBuffer>      ModuleBuffer;

	// 이펙트에 대한 버퍼를 바인딩할 레지스터 번호
	static constexpr int EffectBufferRegNum = 20;
};

struct FRibbonPointData
{
	// 중앙점 위치
	XMFLOAT3 PointPos;
	// 왼쪽 점 위치
	XMFLOAT3 UpPointPos;
	// 오른쪽 점 위치
	XMFLOAT3 DownPointPos;

	float RemainTime;
};

// 리본의 경우 일반적인 파티클과 다른 Tick과 렌더방식을 가지기에 상속하여 가상함수로 진행
class FNiagaraRibbonEmitter : public FNiagaraEmitter
{
public:
	FNiagaraRibbonEmitter();

	std::shared_ptr<FNiagaraEmitter> GetEmitterInstance() const override;
	void                             CreateAndAddNewRibbonPoint(XMFLOAT3 PointPos, XMVECTOR PointRot);

	void Tick(float DeltaSeconds, const FTransform& SceneTransform) override;
	void Render() const override;

	void SetRibbonWidth(int InRibbonWidth)
	{
		RibbonWidth = InRibbonWidth;
	}

	void SetRibbonFaceCamera(bool bInIsBillboard)
	{
		bIsBillboard = bInIsBillboard;
	}

	void SetRibbonColor(XMFLOAT4 InRibbonColor)
	{
		RibbonColor = InRibbonColor;
	}

	void LoadDataFromFile(const nlohmann::basic_json<>& Data) override;

protected:
	void     MapPointDataToVertexBuffer();
	int      RibbonWidth  = 5.0f;
	bool     bIsBillboard = false;
	XMFLOAT4 RibbonColor{1, 1, 1, 1};

	// Array를 환형 큐처럼 사용
	std::array<FRibbonPointData, MaxRibbonPointCount> RibbonPointData;
	int                                               CurRibbonPointDataStartIndex = 0;

	int      CurPointCount = 0;
	XMVECTOR LastFrameWorldPos{0, 0, 0};
	bool     bFirstTick = true;

	// 기존에는 static 으로 고안했으나,
	// 만약 내 리본 점의 개수가 변경되지 않는 경우에는
	// 버텍스버퍼의 데이터를 조정하지 않도록 만들 수 있기에
	// 정적으로 관리하지 않고 멤버변수로 배치
	ComPtr<ID3D11Buffer> VB_Ribbon;

	int CurVertexBufferPointCount = 0;
	int CurVertexCount;
};
