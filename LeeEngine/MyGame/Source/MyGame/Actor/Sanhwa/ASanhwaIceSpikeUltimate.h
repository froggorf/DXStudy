#pragma once
#include "ASanhwaIceBase.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"

class ASanhwaIceSpikeUltimate : public AIceSpikeBase
{
	MY_GENERATE_BODY(ASanhwaIceSpikeUltimate)

public:
	ASanhwaIceSpikeUltimate();
	~ASanhwaIceSpikeUltimate() override = default;

protected:
	std::string GetIceSpikesStaticMeshName() const override { return "SM_Sanhwa_IceSpikes_Ult"; }
		
private:
	std::shared_ptr<UStaticMeshComponent> SM_IceSpikes;

};