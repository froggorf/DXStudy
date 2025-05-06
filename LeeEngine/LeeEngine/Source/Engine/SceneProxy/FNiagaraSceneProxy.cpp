#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include <stdbool.h>

#include "Engine/Mesh/UStaticMesh.h"

std::shared_ptr<FTickParticleCS> FNiagaraSceneProxy::TickParticleCS;

FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID, std::shared_ptr<FNiagaraEmitter> InEmitter)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	if(nullptr == TickParticleCS)
	{
		TickParticleCS = std::make_shared<FTickParticleCS>();	
	}

	Emitter = InEmitter;
	MaterialInterface = Emitter->RenderData->GetMaterialInterface();
	if(std::shared_ptr<UMaterialInstance>MI =  std::dynamic_pointer_cast<UMaterialInstance>(MaterialInterface))
	{
		MI->OverrideTextures;
	}
	//MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraBillboardSprite");

	ParticleBuffer = std::make_shared<FStructuredBuffer>();
	ParticleBuffer->Create(sizeof(FParticleData), MaxParticleCount, SB_TYPE::SRV_UAV, false);
	SpawnBuffer = std::make_shared<FStructuredBuffer>();
	SpawnBuffer->Create(sizeof(FParticleSpawn), 1, SB_TYPE::SRV_UAV, true);

	/**/

	ModuleBuffer = std::make_shared<FStructuredBuffer>();
	ModuleBuffer->Create(sizeof(FParticleModule), 1, SB_TYPE::SRV_ONLY, true,&Emitter->Module);


	AccTime = 0.0f;

	

}



void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();

	ParticleBuffer->Binding(20);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();


	Emitter->RenderData->Render();
}

void FNiagaraSceneProxy::TickCS(float DeltaSeconds)
{
	// 오브젝트의 월드 좌표 계산
	Emitter->Module.ObjectWorldPos = ComponentToWorld.GetTranslation();

	// 이번 프레임에 활성화 될 파티클 수 계산
	CalcSpawnCount(DeltaSeconds);

	ModuleBuffer->SetData(&Emitter->Module);
	// SpawnCount, ParticleBuffer, ModuleBuffer 연결하고
	TickParticleCS->SetSpawnBuffer(SpawnBuffer);
	TickParticleCS->SetParticleBuffer(ParticleBuffer);
	TickParticleCS->SetModuleBuffer(ModuleBuffer);

	TickParticleCS->Execute_Immediately();

}

void FNiagaraSceneProxy::CalcSpawnCount(float DeltaSeconds)
{

	AccTime += DeltaSeconds;
	float Term = 1.f / Emitter->Module.SpawnRate;

	FParticleSpawn Count{};
	if(bFirstTick)
	{
		Count.SpawnCount = 1;
		bFirstTick = false;
	}
	if (AccTime >= Term)
	{
	    AccTime -= Term;
		Count.SpawnCount = 1;
	}

	if(Emitter->Module.Module[static_cast<int>(EParticleModule::PM_SPAWN_BURST)] && 0 < Emitter->Module.SpawnBurstRepeat)
	{
		Emitter->Module.AccSpawnBurstRepeatTime += DeltaSeconds;
		if(Emitter->Module.SpawnBurstRepeatTime < Emitter->Module.AccSpawnBurstRepeatTime)
		{
			Count.SpawnCount += Emitter->Module.SpawnBurstCount;
			Emitter->Module.SpawnBurstRepeat -= 1;
			Emitter->Module.AccSpawnBurstRepeatTime-= Emitter->Module.SpawnBurstRepeatTime;
		}
	}

	if(0< Count.SpawnCount)
	{
		SpawnBuffer->SetData(&Count);
	}
}
