// 05.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "FPrimitiveSceneProxy.h"
#include "Engine/Niagara/FNiagaraEmitter.h"

class FNiagaraSceneProxy : public FPrimitiveSceneProxy
{
public:
	// 인자로 받는 InEmitter는 CreateDynamicRenderData로 만들어진 데이터로 그대로 전달하면 됨
	FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter);
	~FNiagaraSceneProxy() override = default;

	// 렌더링
	void Draw() override;
	// ComputeShader Tick
	void TickCS(float DeltaSeconds);

	// 이펙트 활성화 / 비활성화
	void Activate();
	void Deactivate();

	bool GetIsSetSystemParam() const {return bIsSetSystemParam; }
	void SetSystemParam(const FSystemParamConstantBuffer& NewSystemParam)
	{
		bIsSetSystemParam = true;
		SystemParam = NewSystemParam;
	}
	std::function<void()> DelegateOnTickCS;
protected:
	// Niagara Emitter
	std::shared_ptr<FNiagaraEmitter> Emitter;

	bool bIsActivate        = true;
	bool bMustTickThisFrame = false;

	FSystemParamConstantBuffer SystemParam;
	bool bIsSetSystemParam;
};
