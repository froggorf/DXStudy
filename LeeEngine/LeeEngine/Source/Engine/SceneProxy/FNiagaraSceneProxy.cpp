#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include <stdbool.h>

#include "Engine/Mesh/UStaticMesh.h"


FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	Emitter = InEmitter;
	MaterialInterface = Emitter->RenderData->GetMaterialInterface();
}



void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();
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
	if(bIsActivate || bMustTickThisFrame || Emitter->bUpdateAndRenderAtDeactivate)
	{
		Emitter->Tick(DeltaSeconds, ComponentToWorld);	
		bMustTickThisFrame = false;
		// 리셋 상태였다면 다시 Activate 상태로 바꿔주기
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
