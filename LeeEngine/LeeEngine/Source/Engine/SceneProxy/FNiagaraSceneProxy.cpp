#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include <stdbool.h>

#include "Engine/Mesh/UStaticMesh.h"


FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	

	Emitter = InEmitter;
	MaterialInterface = Emitter->RenderData->GetMaterialInterface();
	if(std::shared_ptr<UMaterialInstance>MI =  std::dynamic_pointer_cast<UMaterialInstance>(MaterialInterface))
	{
		MI->OverrideTextures;
	}

	

}



void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();

	Emitter->Render();
	
}

void FNiagaraSceneProxy::TickCS(float DeltaSeconds)
{
	// 오브젝트의 월드 좌표 계산
	Emitter->Module.ObjectWorldPos = ComponentToWorld.GetTranslation();

	

	Emitter->Tick(DeltaSeconds);

}