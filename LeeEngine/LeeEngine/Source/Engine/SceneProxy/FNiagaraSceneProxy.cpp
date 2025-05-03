#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter)
	:FPrimitiveSceneProxy(InPrimitiveID)
{
	MaterialInterface = InEmitter->RenderProperty->GetMaterial();
	Emitter = InEmitter;
}

FNiagaraSceneProxy::~FNiagaraSceneProxy()
{
}


void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();

	Emitter->RenderProperty->Render();
}
