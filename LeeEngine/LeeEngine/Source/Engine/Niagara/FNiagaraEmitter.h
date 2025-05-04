// 05.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "Engine/Material/UMaterial.h"

// 파티클 구조체
struct FParticleData
{
	XMFLOAT3	LocalPos;		// 소유 오브젝트로 부터의 상대 좌표
	XMFLOAT3	WorldPos;		// 파티클의 월드 좌표
	XMFLOAT3	WorldInitScale; // 파티클 생성 시 초기 크기
	XMFLOAT3	WorldScale;		// 파티클 월드 크기
	XMFLOAT4	Color;			// 파티클 색상

	XMFLOAT3	Force;			// 파티클에 주어지고 있는 힘의 총합
	XMFLOAT3	Velocity;		// 파티클 속도
	float	Mass;			// 파티클 질량

	float	Age;			// 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float	Life;			// 파티클 최대 수명
	float	NormalizedAge;  // 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int		Active;			// 파티클 활성화 여부
};

// 파티클에 적용될 각각의 기능들
enum class EParticleModule
{
	PM_SPAWN,
	PM_SPAWN_BURST,
	PM_ADD_VELOCITY,
	PM_SCALE,
	PM_DRAG,
	PM_RENDER,

	PM_END,
};
// Particle Module
struct FParticleModule
{
	// Spawn Modlue
	float	SpawnRate{};			// 초당 파티클 생성량
	XMFLOAT4	StartColor{};			// 초기 파티클 색상

	XMFLOAT3	MinScale{};			// 생성 시 최소 크기
	XMFLOAT3	MaxScale{};			// 생성 시 최대 크기
	float	MinLife{};			// 생성 시 최소 주기
	float	MaxLife{};			// 생성 시 최대 주기
	int		SpawnShape{};			// 0 : Box, 1 : Sphere
	XMFLOAT3 SpawnShapeScale{};
	int		SpaceType{};			// 0 : Local, 1 : World

	UINT	BlockSpawnShape{};		// 0 : Box,  1: Sphere
	XMFLOAT3	BlockSpawnShapeScale{};	// SpawnShapeScale.x == Radius

	// Spawn Burst
	UINT	SpawnBurstCount{};		// 한번에 발생시키는 Particle 수
	UINT	SpawnBurstRepeat{};
	float	SpawnBurstRepeatTime{};
	float   AccSpawnBurstRepeatTime{};


	// Add Velocity
	UINT	AddVelocityType{};		// 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	XMFLOAT3	AddVelocityFixedDir{};
	float	AddMinSpeed{};
	float	AddMaxSpeed{};

	// Scale Module
	float	StartScale{};
	float	EndScale{};

	// Drag Module (감속 모듈)
	float	DestNormalizedAge{};
	float	LimitSpeed{};

	// Render Module
	XMFLOAT4	EndColor{};			// 파티클 최종 색상
	int		FadeOut{};			// 0 : Off, 1 : Normalized Age
	float   StartRatio{};			// FadeOut 효과가 시작되는 Normalized Age 지점
	UINT	VelocityAlignment{};  // 속도 정렬 0 : Off, 1 : On
	UINT	CrossMesh{};			// 십자형태 메쉬 사용 0 : Off, 1 : ON

	// 추가 데이터
	XMFLOAT3 ObjectWorldPos{};

	// Module On / Off
	int		Module[(UINT)EParticleModule::PM_END] = {0,};
};


class FNiagaraRendererProperty
{
public:
	FNiagaraRendererProperty() = default;
	virtual ~FNiagaraRendererProperty() = default;

	virtual void Render() = 0;

	virtual std::shared_ptr<UMaterialInterface> GetMaterialInterface() const { return MaterialInterface; }
	virtual void SetParticleTexture(const std::shared_ptr<UTexture>& InTexture) {}

protected:
	std::shared_ptr<UMaterialInterface> MaterialInterface;
};

class FNiagaraRendererBillboardSprites : public FNiagaraRendererProperty
{
public:
	FNiagaraRendererBillboardSprites()
	{
		MaterialInterface = UMaterial::GetMaterialCache("MI_NiagaraBillboardSprite");
	}
	~FNiagaraRendererBillboardSprites() override = default;

	void Render() override;

	void SetParticleTexture(const std::shared_ptr<UTexture>& InTexture) override{OverrideSpriteTexture = InTexture;}

	std::shared_ptr<UTexture> OverrideSpriteTexture;
};



class FNiagaraEmitter
{
public:
	FNiagaraEmitter()
	{
		Module.SpawnShape = 0;
		Module.SpawnShapeScale = XMFLOAT3{50.0f,50.0f,50.0f};


		Module.Module[static_cast<int>(EParticleModule::PM_SCALE)] = 1;
		Module.StartScale = 0.5f;
		Module.EndScale = 1.0f;

		Module.FadeOut = 1;
		Module.StartRatio=0.5f;
	}
	~FNiagaraEmitter() = default;

	FParticleModule Module;

	std::shared_ptr<FNiagaraRendererProperty> RenderData;

};
