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
		std::shared_ptr<FNiagaraEmitter> NewEmitter;
		if(PropertyType == 3)
		{
			NewEmitter = std::make_shared<FNiagaraRibbonEmitter>();
		}
		else
		{
			NewEmitter = std::make_shared<FNiagaraEmitter>();
		}

		switch (PropertyType)
		{
			// 0: BillboardSprite
		case 0:
			NewEmitter->RenderData = std::make_shared<FNiagaraRendererBillboardSprites>();
			break;
			// 1 : Sprite
		case 1:
			NewEmitter->RenderData = std::make_shared<FNiagaraRendererSprites>();
			break;
			// 2 : Mesh
		case 2:
			NewEmitter->RenderData=std::make_shared<FNiagaraRendererMeshes>();
			break;
		// 3 : Ribbon
		case 3:
			NewEmitter->RenderData = std::make_shared<FNiagaraRendererRibbons>();
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

		// Rotation
		if(EmitterData.contains("Rot"))
		{
			const auto& RotData = EmitterData["Rot"];
			const auto& MinRotData = RotData[0];
			const auto& MaxRotData = RotData[1];
			NewEmitter->Module.MinRotation = XMFLOAT3{MinRotData[0],MinRotData[1],MinRotData[2]};
			NewEmitter->Module.MaxRotation = XMFLOAT3{MaxRotData[0],MaxRotData[1],MaxRotData[2]};
		}

		//SpaceType
		if (EmitterData.contains("SpaceType"))
		{
			NewEmitter->Module.SpaceType = EmitterData["SpaceType"];
		}

		// Spawn Shape
		if(EmitterData.contains("SpawnShape"))
		{
			NewEmitter->Module.SpawnShape = EmitterData["SpawnShape"];
		}

		// SpawnShapeScale
		if(EmitterData.contains("SpawnScale"))
		{
			const auto& SpawnShapeScale = EmitterData["SpawnScale"];
			NewEmitter->Module.SpawnShapeScale = XMFLOAT3{SpawnShapeScale[0],SpawnShapeScale[1],SpawnShapeScale[2]};
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
					NewEmitter->Module.StartRatio = FadeData[1];
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

			// AddVelocity 모듈
			if(ModuleData.contains("AddVelocity"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_ADD_VELOCITY)]=1;
				const auto& UVData  = ModuleData["AddVelocity"];
				const auto& MinVel = UVData["MinVel"];
				const auto& MaxVel = UVData["MaxVel"];
				NewEmitter->Module.AddMinSpeed = XMFLOAT3{MinVel[0],MinVel[1],MinVel[2]};
				NewEmitter->Module.AddMaxSpeed = XMFLOAT3{MaxVel[0],MaxVel[1],MaxVel[2]};
			}

			// Add Rotation 모듈
			if(ModuleData.contains("AddRot"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_AddRotation)] = 1;
				const auto& AddRotData = ModuleData["AddRot"];
				NewEmitter->Module.AddRotation = XMFLOAT3{AddRotData[0],AddRotData[1],AddRotData[2]};
			}

			// AddTickVelocity 모듈
			if(ModuleData.contains("TickVel"))
			{
				NewEmitter->Module.Module[static_cast<int>(EParticleModule::PM_AddTickVelocity)] = 1;
				const auto& AddTickVelData = ModuleData["TickVel"];
				NewEmitter->Module.AddTickVelocity = XMFLOAT3{AddTickVelData[0],AddTickVelData[1],AddTickVelData[2]};
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
