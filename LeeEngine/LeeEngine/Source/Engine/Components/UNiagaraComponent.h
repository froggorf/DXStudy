﻿// 05.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once

#include "UPrimitiveComponent.h"
#include "Engine/Niagara/UNiagaraSystem.h"

class FNiagaraSceneProxy;

class UNiagaraComponent : public UPrimitiveComponent
{
	MY_GENERATE_BODY(UNiagaraComponent)

	UNiagaraComponent();
	~UNiagaraComponent() override = default;
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> CreateSceneProxy() override;

	std::vector<std::shared_ptr<FNiagaraSceneProxy>> SceneProxies;

	void TickComponent(float DeltaSeconds) override;

	// 이펙트를 활성화 / 비활성화 시키는 함수
	void Activate();
	void Deactivate();

	void SetNiagaraAsset(const std::shared_ptr<UNiagaraSystem>& InNiagaraAsset)
	{
		NiagaraAsset = InNiagaraAsset;
	}

#ifdef WITH_EDITOR
	void DrawDetailPanel(UINT ComponentDepth) override;
#endif

protected:
	std::shared_ptr<UNiagaraSystem> NiagaraAsset;
	bool                            bIsActivate = true;
};
