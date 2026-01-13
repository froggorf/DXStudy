// 01.13
// Town 포탈 액터

#pragma once
#include "Engine/GameFramework/AActor.h"

class UBoxComponent;
class UShapeComponent;
class UStaticMeshComponent;

class APortal : public AActor
{
	MY_GENERATE_BODY(APortal)

	APortal();

	void Register() override;
	void BeginPlay() override;

private:
	void OnPortalBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);

	std::shared_ptr<UStaticMeshComponent> PortalMesh;
	std::shared_ptr<UBoxComponent> TriggerBox;
	bool bTriggered = false;
};
