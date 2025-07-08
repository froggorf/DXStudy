#include "CoreMinimal.h"
#include "UCameraComponent.h"

#include "Engine/RenderCore/RenderingThread.h"


UCameraComponent::UCameraComponent()
{
	Rename("CameraComponent_" + ComponentID);
}

void UCameraComponent::BeginPlay()
{
	USceneComponent::BeginPlay();

	ViewMatrices.UpdateProjectionMatrix(XMMatrixPerspectiveFovLH(0.5*XM_PI, AspectRatio, 0.1f, 1000.0f));
}

void UCameraComponent::UpdateCameraData()
{
 	const FTransform& ComponentToWorld = GetComponentTransform();
	ViewMatrices.UpdateViewMatrix(ComponentToWorld.GetTranslation(), ComponentToWorld.GetRotationQuat());

	FScene::UpdateViewMatrix_GameThread(ViewMatrices);
}