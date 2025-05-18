#include "CoreMinimal.h"
#include "UNiagaraSystem.h"

void UNiagaraSystem::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	const auto& EmittersData = AssetData["Emitters"];
	for (const auto& EmitterData : EmittersData)
	{
		int PropertyType = EmitterData["Property"];
		// 리본렌더러의 경우에는 FNiagaraRibbonEmitter를 사용
		std::shared_ptr<FNiagaraEmitter> NewEmitter = PropertyType == 3 ?
														std::make_shared<FNiagaraRibbonEmitter>() :
														std::make_shared<FNiagaraEmitter>();
		NewEmitter->LoadDataFromFile(EmitterData);
		this->Emitters.push_back(NewEmitter);
	}

	GetNiagaraAssetCacheMap()[GetName()] = shared_from_this();
}

std::vector<std::shared_ptr<FNiagaraEmitter>> UNiagaraSystem::CreateDynamicRenderData() const
{
	if (Emitters.empty())
	{
		return {};
	}

	std::vector<std::shared_ptr<FNiagaraEmitter>> EmitterRenderData;
	for (const auto& Emitter : Emitters)
	{
		// 05.08, 여기서 FNiagaraEmitter를 직접 만들어 값을 복사하여 반환했는데,
		// 이럴경우 FNiagaraRibbonEmitter의 가상함수 Render가 실행이 안되어 이미터에서 인스턴스를 만들어 반환하도록 구조 변경
		auto NewEmitter = Emitter->GetEmitterInstance();
		EmitterRenderData.push_back(NewEmitter);
	}
	return EmitterRenderData;
}
