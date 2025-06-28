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
	
	void OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults);

protected:
	std::shared_ptr<UStaticMeshComponent> TestCubeStaticMeshComp;

	std::shared_ptr<UStaticMeshComponent> TestCubeSM1;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM2;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM3;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM4;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM5;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM6;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM9;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM7;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM8;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM10;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM11;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM12;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM13;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM14;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM15;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM16;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM17;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM18;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM19;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM20;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM21;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM22;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM23;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM24;
	std::shared_ptr<UStaticMeshComponent> TestCubeSM25;

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
