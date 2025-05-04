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

		// Override Material 세팅
		if(EmitterData.contains("OverrideMat"))
		{
			std::string_view MaterialName = EmitterData["OverrideMat"];
			NewEmitter->RenderData->SetMaterialInterface(UMaterialInterface::GetMaterialCache(MaterialName.data()));
		}

		// Override Texture 세팅
		if(EmitterData.contains("OverrideTex"))
		{
			std::string_view TextureName = EmitterData["OverrideTex"];
			// 머테리얼 인스턴스에 등록을 하는 방향으로 하려했으나,
			// 새로 머테리얼인스턴스를 생성하는 방식으로 인해 실패
			// 따라서 RenderData 내에 데이터를 넣고, 렌더링 시 바인딩 하는 방향으로 진행
			NewEmitter->RenderData->SetParticleTexture(UTexture::GetTextureCache(TextureName.data()));

		}

		// SpawnRate
		if (EmitterData.contains("SpawnRate"))
		{
			NewEmitter->Module.SpawnRate = EmitterData["SpawnRate"];
		}

		// Life
		if (EmitterData.contains("Life"))
		{
			auto LifeData = EmitterData["Life"];
			NewEmitter->Module.MinLife = LifeData[0];
			NewEmitter->Module.MaxLife = LifeData[1];
		}

		// Scale
		if (EmitterData.contains("Scale"))
		{
			auto ScaleData = EmitterData["Scale"];
			auto MinScaleData = ScaleData[0];
			auto MaxScaleData = ScaleData[1];
			NewEmitter->Module.MinScale = XMFLOAT3{ MinScaleData[0],MinScaleData[1],MinScaleData[2] };
			NewEmitter->Module.MaxScale = XMFLOAT3{ MaxScaleData[0],MaxScaleData[1],MaxScaleData[2] };
		}

		//SpaceType
		if (EmitterData.contains("SpaceType"))
		{
			NewEmitter->Module.SpaceType = EmitterData["SpaceType"];
		}


		// 모듈값 지정 시작
		if(EmitterData.contains("Modules"))
		{
			auto ModuleData = EmitterData["Modules"];

			//Render 모듈
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
				if (RenderData.contains("FadeOut"))
				{
					const auto& FadeData = RenderData["FadeOut"];
					NewEmitter->Module.FadeOut = FadeData[0];
					NewEmitter->Module.FadeOut = FadeData[1];
				}
			}

			// Scale 모듈
			if (ModuleData.contains("Scale"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_SCALE)] = 1;
				auto ScaleData = ModuleData["Scale"];
				NewEmitter->Module.StartScale = ScaleData["StartScale"];
				NewEmitter->Module.EndScale = ScaleData["EndScale"];
			}

			// UVAnim 모듈
			if(ModuleData.contains("UVAnim"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_UVAnim)]=1;
				const auto& UVData  = ModuleData["UVAnim"];
				NewEmitter->Module.UCount = UVData["UCount"];
				NewEmitter->Module.VCount = UVData["VCount"];
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
