// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UBoxComponent.h"
#include "Engine/Physics/UConvexComponent.h"

class UStaticMeshComponent;

class ATestCube : public AActor
{
	MY_GENERATE_BODY(ATestCube)

	ATestCube();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	
	void OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults);
	void OnComponentBeginOverlapEvent(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp );

	void ShowDebug();
	void DoRecast();

	static rcPolyMesh* MyPolyMesh;
	static rcPolyMeshDetail* MyPolyDetail;
	static dtNavMesh* MyDtNavMesh;
	static dtNavMeshQuery* MyDtNavQuery;
protected:
	std::shared_ptr<UStaticMeshComponent> SM_Well_PBR;

	std::shared_ptr<ULightComponent> Light1;
	std::shared_ptr<ULightComponent> Light2;
	std::shared_ptr<ULightComponent> Light3;
	std::shared_ptr<ULightComponent> Light4;

	std::shared_ptr<UDecalComponent> DecalTest1;

	std::shared_ptr<UStaticMeshComponent> TestCubeStaticMeshComp;

	std::shared_ptr<UStaticMeshComponent> SM_DeferredSphere[5];


	std::shared_ptr<UBoxComponent> TriggerBox1;

private:
	std::shared_ptr<UStaticMeshComponent> TestCube2;
	std::shared_ptr<UStaticMeshComponent> TestCube3;
	std::shared_ptr<UStaticMeshComponent> TestCube4;


	std::shared_ptr<UNiagaraComponent> NC_BillboardRibbon;
	std::shared_ptr<USceneComponent>   DummyComp;

	std::shared_ptr<UNiagaraComponent> NC_Ribbon;
	std::shared_ptr<USceneComponent>   DummyComp2;

	std::shared_ptr<UNiagaraComponent> NC_Fire;

	std::shared_ptr<UConvexComponent> RecastConvexMesh;


	void CreateDetour();
};
