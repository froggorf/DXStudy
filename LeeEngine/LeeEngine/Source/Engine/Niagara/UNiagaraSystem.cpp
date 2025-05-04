#include "CoreMinimal.h"
#include "UNiagaraSystem.h"

void UNiagaraSystem::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	const auto& EmittersData = AssetData["Emitters"];
	for (const auto& EmitterData : EmittersData)
	{
		int PropertyType = EmitterData["Property"];
		auto NewEmitter = std::make_shared<FNiagaraEmitter>();

		switch (PropertyType)
		{
			// 0: BillboardSprite
		case 0:
			NewEmitter->RenderData = std::make_shared<FNiagaraRendererBillboardSprites>();
			break;
			// 1 : Sprite
		case 1:
			break;
			// 2 : Mesh
		case 2:
		break;
		// 3 : Ribbon
		case 3:
		break;

		default:
			assert(0 && "잘못된 PropertyType");
			break;
		}

		// 모듈값 지정 시작
		// StartColor
		if(EmitterData.contains("Modules"))
		{
			auto ModuleData = EmitterData["Modules"];
			if(ModuleData.contains("Render"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_RENDER)] = 1;
				auto RenderData = ModuleData["Render"];
				if (RenderData.contains("StartColor"))
				{
					auto Colors = RenderData["StartColor"];
					NewEmitter->Module.StartColor = XMFLOAT4{ Colors[0],Colors[1],Colors[2],Colors[3] };
				}
				if(RenderData.contains("EndColor"))
				{
					auto Colors = RenderData["EndColor"];
					NewEmitter->Module.EndColor = XMFLOAT4{ Colors[0],Colors[1],Colors[2],Colors[3] };
				}
			}
		}
		
		this->Emitters.push_back(NewEmitter);
		
	}

	GetNiagaraAssetCacheMap()[GetName()] =  shared_from_this();

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
		auto NewEmitter = std::make_shared<FNiagaraEmitter>();
		NewEmitter->Module = Emitter->Module;
		NewEmitter->RenderData = Emitter->RenderData;
		EmitterRenderData.push_back(NewEmitter);
	}
	return EmitterRenderData;
}
