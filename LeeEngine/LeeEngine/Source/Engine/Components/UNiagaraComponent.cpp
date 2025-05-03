#include "CoreMinimal.h"
#include "UNiagaraComponent.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"
#include "Engine/World/UWorld.h"

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UNiagaraComponent::CreateSceneProxy()
{
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;
	std::shared_ptr<FNiagaraSceneProxy> NiagaraSceneProxy = std::make_shared<FNiagaraSceneProxy>(GetPrimitiveID());
	SceneProxies.emplace_back(NiagaraSceneProxy);

	this->SceneProxies.emplace_back(NiagaraSceneProxy);

	return SceneProxies;
}

void UNiagaraComponent::TickComponent(float DeltaSeconds)
{
	UPrimitiveComponent::TickComponent(DeltaSeconds);

	for (const auto& SceneProxy : SceneProxies)
	{
		GEngine->GetWorld()->AddToBeTickedNiagaraSceneProxy(SceneProxy);
	}
	
}
