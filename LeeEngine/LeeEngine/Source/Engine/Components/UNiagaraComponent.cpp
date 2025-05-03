#include "CoreMinimal.h"
#include "UNiagaraComponent.h"

#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

UNiagaraComponent::UNiagaraComponent()
{
	Rename("NiagaraComponent_" + std::to_string(ComponentID));
}

UNiagaraComponent::~UNiagaraComponent()
{
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UNiagaraComponent::CreateSceneProxy() const
{
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;

	// 나이아가라 렌더 데이터를 Create Dynamic
	const auto& NiagaraRenderData = NiagaraAsset->CreateDynamicRenderData();
	for(const auto& RenderData : NiagaraRenderData)
	{
		// 해당 데이터를 NiagaraSceneProxy에 전달
		std::shared_ptr<FNiagaraSceneProxy> SceneProxy = std::make_shared<FNiagaraSceneProxy>(PrimitiveID, RenderData);
		SceneProxies.emplace_back(SceneProxy);	
	}
	
	return SceneProxies;

}

void UNiagaraComponent::SetNiagaraAsset(const std::shared_ptr<UNiagaraSystem>& NewNiagaraSystem)
{
	NiagaraAsset = NewNiagaraSystem;
}
