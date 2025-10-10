#include "CoreMinimal.h"
#include "ASanhwaIceBase.h"

ASanhwaIceSpikeBase::ASanhwaIceSpikeBase()
{
	SM_IceSpikes = std::make_shared<UStaticMeshComponent>();
	SM_IceSpikes->SetupAttachment(GetRootComponent());
	SM_IceSpikes->SetRelativeRotation(XMFLOAT3{90.0f,0.0f,0.0f});
	SM_IceSpikes->SetRelativeLocation(XMFLOAT3{0.0f,StartGroundDepth, 0.0f});

}

void ASanhwaIceSpikeBase::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache(GetIceSpikesStaticMeshName(),[this](std::shared_ptr<UObject> Object)
		{
			SM_IceSpikes->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
}

void ASanhwaIceSpikeBase::BeginPlay()
{
	AActor::BeginPlay();

	GEngine->GetTimerManager()->SetTimer(SpawnTimerHandle, {this, &ASanhwaIceSpikeBase::SpawnIce}, 0.0f, true, TimerTickTime);
}

void ASanhwaIceSpikeBase::SpawnIce()
{
	CurrentSpawnTime = std::min(CurrentSpawnTime + TimerTickTime, SpawnTime);

	SM_IceSpikes->SetRelativeLocation({0, std::lerp(StartGroundDepth, 0.0f, CurrentSpawnTime / SpawnTime), 0.0f});

	if (CurrentSpawnTime >= SpawnTime)
	{
		GEngine->GetTimerManager()->ClearTimer(SpawnTimerHandle);
	}
	
}
