#include "CoreMinimal.h"
#include "UNiagaraSystem.h"

void UNiagaraSystem::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);


}

std::vector<std::shared_ptr<FNiagaraEmitter>> UNiagaraSystem::CreateDynamicRenderData() const
{
	std::vector<std::shared_ptr<FNiagaraEmitter>> EmitterRenderData;
	for (const auto& Emitter : Emitters)
	{
		auto NewEmitter = std::make_shared<FNiagaraEmitter>();
		NewEmitter->Module = Emitter->Module;
		NewEmitter->RenderData = Emitter->RenderData;
		EmitterRenderData.push_back(NewEmitter);
	}
	return EmitterRenderData;
}
