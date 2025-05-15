#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include <stdbool.h>

#include "Engine/Mesh/UStaticMesh.h"


FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	Emitter = InEmitter;
	MaterialInterface = Emitter->RenderProperty->GetMaterialInterface();
}



void FNiagaraSceneProxy::Draw()
{
	// 활성화 중이거나 || 리본렌더러 같이 비활성화 시 잔상은 남아야 할 경우 Render 진행
	if(bIsActivate || Emitter->bUpdateAndRenderAtDeactivate)
	{
		Emitter->Render();
	}
}

enum class EParticleActivateState
{
	EPAS_Activate,
	EPAS_Deactivate,
	EPAS_Reset,	
};

void FNiagaraSceneProxy::TickCS(float DeltaSeconds)
{
	// 활성화 중이거나 || 이번 프레임에 틱되어야 하거나 || 리본렌더러 같이 비활성화 시 잔상은 남아야 할 경우 Tick 진행
	if(bIsActivate || bMustTickThisFrame || Emitter->bUpdateAndRenderAtDeactivate)
	{
		Emitter->Tick(DeltaSeconds, ComponentToWorld);

		bMustTickThisFrame = false;
		// 리셋 상태였다면 다시 Activate 상태로 바꿔주기
		// ComputeShader 내에선 Reset 시 파티클을 재생성하는 로직을 가졌기 때문에, Reset에서 Activate로 바꿔주지 않으면 계속 지우고 생성되고의 반복이 되므로
		// Activate로 만들어줌
		if(Emitter->Module.ActivateState == static_cast<int>(EParticleActivateState::EPAS_Reset))
		{
			Emitter->Module.ActivateState = static_cast<int>(EParticleActivateState::EPAS_Activate);
		}
	}
}


void FNiagaraSceneProxy::Activate()
{
	bIsActivate = true;
	Emitter->Module.ActivateState = Emitter->Module.ActivateState == static_cast<int>(EParticleActivateState::EPAS_Deactivate) ? 
														static_cast<int>(EParticleActivateState::EPAS_Reset) :
														static_cast<int>(EParticleActivateState::EPAS_Activate);
	bMustTickThisFrame = true;
	Emitter->Reset();
}

void FNiagaraSceneProxy::Deactivate()
{
	bIsActivate = false;
	bMustTickThisFrame = true;
	Emitter->Module.ActivateState = static_cast<int>(EParticleActivateState::EPAS_Deactivate);
}
