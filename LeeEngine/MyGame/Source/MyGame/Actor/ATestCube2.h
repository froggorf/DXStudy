// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/PBRTestComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/GameFramework/AActor.h"

class UDecalComponent;
class UNiagaraComponent;
class UStaticMeshComponent;

class ATestCube2 : public AActor
{
	MY_GENERATE_BODY(ATestCube2)

	ATestCube2();

	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

protected:


	std::shared_ptr<UStaticMeshComponent> Ground;

private:
	std::shared_ptr<AAIController> AIController;


};
