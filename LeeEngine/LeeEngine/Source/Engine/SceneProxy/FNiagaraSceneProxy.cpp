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

	Emitter->Render();
	
}

void FNiagaraSceneProxy::TickCS(float DeltaSeconds)
{
	// 오브젝트의 월드 좌표 계산

	

	Emitter->Tick(DeltaSeconds, ComponentToWorld);

}