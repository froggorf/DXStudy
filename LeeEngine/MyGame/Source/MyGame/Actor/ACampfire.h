#pragma once

#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;
class UNiagaraComponent;
class ULightComponent;

class ACampfire : public AActor
{
	MY_GENERATE_BODY(ACampfire)

	ACampfire();
	void Register() override;

private:
	std::shared_ptr<UStaticMeshComponent> CampfireMesh;
	std::shared_ptr<UNiagaraComponent>    FireVFX;
	std::shared_ptr<ULightComponent>      FireLight;
};
