// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "FPrimitiveSceneProxy.h"
#include "Engine/Niagara/UNiagaraSystem.h"

class UNiagaraSystem;
class UNiagaraComponent;

class FNiagaraSceneProxy : public FPrimitiveSceneProxy
{
public:
	FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter);
	~FNiagaraSceneProxy() override;



	void Draw() override;

private:
	// NiagaraSystem 내에서 
	std::shared_ptr<FNiagaraEmitter> Emitter;
};
