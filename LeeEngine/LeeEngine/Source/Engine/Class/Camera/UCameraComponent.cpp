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
}

void UCameraComponent::TickComponent(float DeltaSeconds)
{
	USceneComponent::TickComponent(DeltaSeconds);

	UpdateCameraMatrices();
}

void UCameraComponent::UpdateCameraMatrices()
{
	const FTransform& ComponentToWorld = GetComponentTransform();

	switch (ProjectType)
	{
	case EProjectionType::OrthoGraphic:
		ViewMatrices.UpdateProjectionMatrix(XMMatrixOrthographicLH(Width * OrthoScale, Width / AspectRatio * OrthoScale, Near, Far));
		break;
	case EProjectionType::Perspective:
		AspectRatio = GDirectXDevice->GetResolution().x / GDirectXDevice->GetResolution().y;
		ViewMatrices.UpdateProjectionMatrix(XMMatrixPerspectiveFovLH(FOV, AspectRatio, Near, Far));
	break;
	default:
		break;
	}

	ViewMatrices.UpdateViewMatrix(ComponentToWorld.GetTranslation(), ComponentToWorld.GetRotationQuat());

	
	
}

void UCameraComponent::UpdateCameraData()
{
	UpdateCameraMatrices();
	FScene::UpdateViewMatrix_GameThread(ViewMatrices);
}
