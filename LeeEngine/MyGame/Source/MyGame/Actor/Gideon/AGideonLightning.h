#pragma once
#include "Engine/GameFramework/AActor.h"
#include "MyGame/Character/Player/AGideonCharacter.h"

class AGideonLightning : public AActor
{
	MY_GENERATE_BODY(AGideonLightning)

public:
	AGideonLightning();
	~AGideonLightning() override = default;
	void Register() override;
	void BeginPlay() override;


protected:

private: 
};
