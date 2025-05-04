// 05.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "FPrimitiveSceneProxy.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/Niagara/FNiagaraEmitter.h"

#define MaxParticleCount 500
#define ParticleDataRegister 20


struct FParticleSpawn
{
	UINT	SpawnCount;	
	float arrPaddding[3];
};

class FNiagaraSceneProxy : public FPrimitiveSceneProxy
{
public:
	// 인자로 받는 InEmitter는 CreateDynamicRenderData로 만들어진 데이터로 그대로 전달하면 됨
	FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter);
	~FNiagaraSceneProxy() override = default;

	void Draw() override;
	void TickCS(float DeltaSeconds);
	void CalcSpawnCount(float DeltaSeconds);

public:
protected:
	std::shared_ptr<FNiagaraEmitter> Emitter;
	//FParticleModule Module;
	float AccTime;

	std::shared_ptr<FStructuredBuffer> ParticleBuffer;
	std::shared_ptr<FStructuredBuffer> SpawnBuffer;
	std::shared_ptr<FStructuredBuffer> ModuleBuffer;

	static std::shared_ptr<FTickParticleCS> TickParticleCS;
};
