#include "CoreMinimal.h"
#include "APlayerController.h"

#include "ACharacter.h"
#include "Engine/UEngine.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/World/UWorld.h"


APlayerController::APlayerController()
{
	static UINT PlayerControllerID = 0;
	Rename("PlayerController_"+ (PlayerControllerID++));
}

void APlayerController::BeginPlay()
{
	AActor::BeginPlay();

	CameraManager = std::dynamic_pointer_cast<APlayerCameraManager>(GEngine->GetWorld()->GetPersistentLevel()->SpawnActor("APlayerCameraManager", {}));
}

void APlayerController::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	ImGuiIO io = ImGui::GetIO();
	ImVec2 MouseDelta = io.MouseDelta;

	XMFLOAT4 Rot = GetActorRotation();
	XMVECTOR ControlRotation = XMLoadFloat4(&Rot);
	
	XMVECTOR ForwardVector = XMVector3Normalize(XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), ControlRotation));
	XMVECTOR UpVector    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR RightVector = XMVector3Normalize(XMVector3Cross(UpVector, ForwardVector));

	

	XMVECTOR LocalPitchQuat = XMQuaternionRotationAxis(RightVector, XMConvertToRadians(MouseDelta.y * DeltaSeconds ));
	XMVECTOR LocalYawQuat   = XMQuaternionRotationAxis(UpVector, XMConvertToRadians(MouseDelta.x * DeltaSeconds));

	XMVECTOR NewControlRotationQuat = XMQuaternionMultiply(ControlRotation, LocalPitchQuat);
	NewControlRotationQuat = XMQuaternionNormalize(XMQuaternionMultiply(NewControlRotationQuat, LocalYawQuat));

	XMStoreFloat4(&Rot, NewControlRotationQuat);
	SetActorRotation(Rot);

	if (Character)
	{
		Character->SetControlRotation(Rot);
	}
}

void APlayerController::OnPossess(ACharacter* CharacterToPossess)
{
	if (CharacterToPossess != nullptr)
	{
		Character = CharacterToPossess;
	}
}

