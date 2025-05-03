// 05.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/UObject/UObject.h"
#include "Engine/Material/UMaterial.h"

struct FParticleData
{
	XMFLOAT3	LocalPos;		// 소유 오브젝트로 부터의 상대 좌표
	XMFLOAT3	WorldPos;		// 파티클의 월드 좌표
	XMFLOAT3	WorldInitScale; // 파티클 생성 시 초기 크기
	XMFLOAT3	WorldScale;		// 파티클 월드 크기
	XMFLOAT4	Color;			// 파티클 색상

	XMFLOAT3  Force;			// 파티클에 주어지고 있는 힘의 총합
	XMFLOAT3  Velocity;			// 파티클 속도
	float	Mass;				// 파티클 질량

	float	Age;				// 파티클 나이, Age 가 Life 에 도달하면 수명이 다한 것
	float	Life;				// 파티클 최대 수명
	float	NormalizedAge;		// 전체 수명 대비, 현재 Age 비율. 자신의 Age 를 Life 대비 정규화 한 값

	int		Active;				// 파티클 활성화 여부
};

// Particle Module
struct tParticleModule
{

};

#define MaxParticleCount 1024
#define ParticleRegister 20

class FNiagaraRendererProperty
{
public:
	FNiagaraRendererProperty()
	{
		ParticleBuffer = std::make_shared<FStructuredBuffer>();
		ParticleBuffer->Create(sizeof(FParticleData), MaxParticleCount, SB_TYPE::SRV_UAV, false);
	};
	virtual ~FNiagaraRendererProperty() = default;

	virtual std::shared_ptr<FNiagaraRendererProperty> CreateDynamicRenderData() = 0;

	virtual void ExecuteCS();
	virtual void Render();


	void SetMaterial(const std::shared_ptr<UMaterialInterface>& InMaterialInterface)
	{
		MaterialInterface = InMaterialInterface;
	}
	std::shared_ptr<UMaterialInterface> GetMaterial() const { return MaterialInterface; }
protected:
	std::shared_ptr<FStructuredBuffer> ParticleBuffer;
	std::shared_ptr<UMaterialInterface> MaterialInterface;
	static std::shared_ptr<FTickParticleCS> TickParticleCS;
};

class FNiagaraRendererSprites : public FNiagaraRendererProperty
{
public:
	FNiagaraRendererSprites()
	{
		if(nullptr == Mesh)
		{
			Mesh = UStaticMesh::GetStaticMesh("SM_Point");	
		}
		MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraBillboardSprite");
	}
	~FNiagaraRendererSprites() override = default;
	std::shared_ptr<FNiagaraRendererProperty> CreateDynamicRenderData() override
	{
		return std::make_shared<FNiagaraRendererSprites>();
	}

	void Render() override;

protected:
private:
	static std::shared_ptr<UStaticMesh> Mesh;
};

class FNiagaraEmitter
{
public:
	FNiagaraEmitter() = default;
	~FNiagaraEmitter() = default;

	std::shared_ptr<FNiagaraRendererProperty> RenderProperty;
};


class UNiagaraSystem : public UObject, public std::enable_shared_from_this<UNiagaraSystem>
{
	MY_GENERATED_BODY(UNiagaraSystem)
public:
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	static std::shared_ptr<UNiagaraSystem> GetNiagaraSystem(const std::string& Name)
	{
		auto Target = GetNiagaraSystemCache().find(Name);
		if (Target != GetNiagaraSystemCache().end())
		{
			return Target->second;
		}
		return nullptr;
	}

	std::vector<std::shared_ptr<FNiagaraEmitter>> CreateDynamicRenderData() const
	{
		std::vector<std::shared_ptr<FNiagaraEmitter>> RenderData;
		for (auto& Emitter : Emitters)
		{
			std::shared_ptr<FNiagaraEmitter> EmitterRenderData = std::make_shared<FNiagaraEmitter>();
			EmitterRenderData->RenderProperty = Emitter->RenderProperty->CreateDynamicRenderData();
			RenderData.push_back(EmitterRenderData);
		}
		return RenderData;
	}
protected:
private:
public:
protected:

	std::vector<std::shared_ptr<FNiagaraEmitter>> Emitters;
private:
	static std::unordered_map<std::string, std::shared_ptr<UNiagaraSystem>>& GetNiagaraSystemCache(){
		static std::unordered_map<std::string, std::shared_ptr<UNiagaraSystem>> NiagaraSystemCache;
		return NiagaraSystemCache;
	}
};
