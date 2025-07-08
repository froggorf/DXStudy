#include "CoreMinimal.h"
#include "APlayerController.h"

#include "Engine/UEngine.h"
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
