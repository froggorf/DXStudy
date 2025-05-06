#include "CoreMinimal.h"
#include "UNiagaraComponent.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"
#include "Engine/World/UWorld.h"

UNiagaraComponent::UNiagaraComponent()
{

	Rename("NiagaraComponent_" + std::to_string(ComponentID));
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UNiagaraComponent::CreateSceneProxy()
{
	if(nullptr == NiagaraAsset)
	{
		return {};
	}

	const auto& RenderData = NiagaraAsset->CreateDynamicRenderData();
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies(RenderData.size());

	for(int i =0 ;i < RenderData.size(); ++i)
	{
		std::shared_ptr<FNiagaraSceneProxy> NiagaraSceneProxy = std::make_shared<FNiagaraSceneProxy>(GetPrimitiveID(),RenderData[i]);
		SceneProxies.emplace_back(NiagaraSceneProxy);
		this->SceneProxies.emplace_back(NiagaraSceneProxy);
	}
	
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
