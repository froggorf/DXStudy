#pragma once
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"

class ASanhwaIceSpikeBase : public AActor
{
	MY_GENERATE_BODY(ASanhwaIceSpikeBase)

public:
	ASanhwaIceSpikeBase();
	~ASanhwaIceSpikeBase() override = default;
	void Register() override;
	void BeginPlay() override;

	void SpawnIce();

	void SetCollision(ECollisionEnabled NewCollision);

protected:
	virtual std::string GetIceSpikesStaticMeshName() const { return "SM_Sanhwa_IceSpikes_Skill"; }
		
private: 
	std::shared_ptr<UStaticMeshComponent> SM_IceSpikes;
	FTimerHandle SpawnTimerHandle;
	static constexpr float StartGroundDepth = -320.0f;
	static constexpr float SpawnTime = 0.2f;
	float CurrentSpawnTime = 0.0f;
	static constexpr float TimerTickTime = 0.005f;
};