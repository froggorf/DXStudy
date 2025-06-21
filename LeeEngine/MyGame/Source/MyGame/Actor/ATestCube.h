// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;

class ATestCube : public AActor
{
	MY_GENERATE_BODY(ATestCube)

	ATestCube();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

protected:
	std::shared_ptr<UStaticMeshComponent> TestCubeStaticMeshComp;

	std::shared_ptr<UStaticMeshComponent> TestRigidSM;

private:
	std::shared_ptr<UStaticMeshComponent> TestCube2;
	std::shared_ptr<UStaticMeshComponent> TestCube3;
	std::shared_ptr<UStaticMeshComponent> TestCube4;

	std::shared_ptr<UNiagaraComponent> NC_BillboardRibbon;
	std::shared_ptr<USceneComponent>   DummyComp;

	std::shared_ptr<UNiagaraComponent> NC_Ribbon;
	std::shared_ptr<USceneComponent>   DummyComp2;

	std::shared_ptr<UNiagaraComponent> NC_Fire;
};
