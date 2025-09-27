#include "CoreMinimal.h"
#include "APlayerCameraManager.h"

static UINT PlayerCameraManagerID = 0;
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

	if (std::shared_ptr<UCameraComponent> CameraComp = TargetCamera.lock())
	{
		CameraComp->UpdateCameraData();
	}
	

	
}
