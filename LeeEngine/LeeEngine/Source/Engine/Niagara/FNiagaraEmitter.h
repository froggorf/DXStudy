// 05.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

class FNiagaraRendererProperty
{
public:
	FNiagaraRendererProperty() = default;
	virtual ~FNiagaraRendererProperty() = default;

	virtual void Render() = 0;
};

class FNiagaraRendererSprites : public FNiagaraRendererProperty
{
public:
	FNiagaraRendererSprites() = default;
	~FNiagaraRendererSprites() override = default;

	void Render() override {}

};

class FNiagaraEmitter
{
public:
	FNiagaraEmitter() = default;
	~FNiagaraEmitter() = default;

	FParticleModule Module;

	std::shared_ptr<FNiagaraRendererProperty> RenderData;

};
