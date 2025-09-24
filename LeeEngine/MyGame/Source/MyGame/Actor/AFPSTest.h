// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Class/Framework/ACharacter.h"
#include "Engine/Class/Light/ULightComponent.h"


class AFPSTest : public AActor
{
	MY_GENERATE_BODY(AFPSTest)
	
	AFPSTest();
	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
public:
	std::shared_ptr<UStaticMeshComponent> SM_Attacker;

	float ShotDelay = 0.5f;
	float CurTime = 0.0f;
};
