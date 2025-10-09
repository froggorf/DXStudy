#include "CoreMinimal.h"
#include "APlayerCameraManager.h"

static UINT PlayerCameraManagerID = 0;
FTransform APlayerCameraManager::LastUpdateCameraTransform;

APlayerCameraManager::APlayerCameraManager()
{
	int a = 0;
	Rename("PlayerCameraManager_"+ (PlayerCameraManagerID++));
}

APlayerCameraManager::~APlayerCameraManager()
{
	int a = 0;
}

void APlayerCameraManager::BeginPlay()
{
	AActor::BeginPlay();

}

void APlayerCameraManager::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	if (bCameraBlending)
	{
		BlendCameraAndUpdateCameraData(DeltaSeconds);
	}
	else
	{
		if (std::shared_ptr<UCameraComponent> CameraComp = TargetCamera.lock())
		{
			LastUpdateCameraTransform = CameraComp->GetComponentTransform();
			CameraComp->UpdateCameraData();
		}	
	}
	
	

	
}

void APlayerCameraManager::SetViewTargetWithBlend(const std::shared_ptr<UCameraComponent>& NewCameraComp, float BlendTime, EViewTargetBlendFunction InBlendFunction)
{
	bCameraBlending = true;
	CurrentCameraBlendTime = 0.0f;
	CameraBlendTime = BlendTime;
	BlendFunction = InBlendFunction;

	if (TargetCamera.expired())
	{
		BlendStartTransform = LastUpdateCameraTransform;
	}
	else
	{
		const std::shared_ptr<UCameraComponent> Camera = TargetCamera.lock();
		BlendStartTransform = Camera->GetComponentTransform();
	}

	TargetBlendingCamera = NewCameraComp;
}

void APlayerCameraManager::BlendCameraAndUpdateCameraData(float DT)
{
	CurrentCameraBlendTime += DT;
	TargetBlendingCamera->UpdateCameraData();
	if (CurrentCameraBlendTime >= CameraBlendTime)
	{
		bCameraBlending = false;
		CurrentCameraBlendTime = CameraBlendTime;
		TargetCamera = TargetBlendingCamera;
		TargetBlendingCamera = nullptr;
		return;
	}

	float Alpha = CameraBlendTime > 0 ? CurrentCameraBlendTime / CameraBlendTime : 1.0f;
	Alpha = std::clamp(Alpha, 0.0f, 1.0f);

	const FTransform& TargetTransform = TargetBlendingCamera->GetComponentTransform();

	XMFLOAT3 StartLocation = BlendStartTransform.GetTranslation();
	XMVECTOR StartLocationVec = XMLoadFloat3(&StartLocation);
	XMFLOAT3 TargetLocation = TargetTransform.GetTranslation();
	XMVECTOR TargetLocationVec = XMLoadFloat3(&TargetLocation);

	XMVECTOR StartRotation = BlendStartTransform.GetRotationQuat();
	XMVECTOR TargetRotation = TargetTransform.GetRotationQuat();

	XMFLOAT3 BlendedLocation;
	XMVECTOR BlendedRotation;

	switch (BlendFunction)
	{
	case EViewTargetBlendFunction::Linear:
		XMStoreFloat3(&BlendedLocation, XMVectorLerp(StartLocationVec, TargetLocationVec, Alpha));
		BlendedRotation = XMQuaternionSlerp(StartRotation, TargetRotation, Alpha);
		break;

	case EViewTargetBlendFunction::Cubic:
	{
		float CubicAlpha = 3 * (Alpha * Alpha) - 2 * (Alpha * Alpha * Alpha);
		XMStoreFloat3(&BlendedLocation, XMVectorLerp(StartLocationVec, TargetLocationVec, CubicAlpha));
		BlendedRotation = XMQuaternionSlerp(StartRotation, TargetRotation, CubicAlpha);
	}
	break;
	default:
		break;
	}

	FViewMatrices NewViewMatrices = TargetBlendingCamera->GetViewMatrices();
	NewViewMatrices.UpdateViewMatrix(BlendedLocation, BlendedRotation);
	FScene::UpdateViewMatrix_GameThread(NewViewMatrices);
}
