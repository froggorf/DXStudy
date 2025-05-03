#include "CoreMinimal.h"
#include "UNiagaraComponent.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UNiagaraComponent::CreateSceneProxy() const
{
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;
	std::shared_ptr<FNiagaraSceneProxy> NiagaraSceneProxy = std::make_shared<FNiagaraSceneProxy>(GetPrimitiveID());
	SceneProxies.emplace_back(NiagaraSceneProxy);

	return SceneProxies;
}
