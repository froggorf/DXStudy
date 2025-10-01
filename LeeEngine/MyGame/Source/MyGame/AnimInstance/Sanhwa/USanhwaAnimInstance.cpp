#include "CoreMinimal.h"
#include "USanhwaAnimInstance.h"

#include "Engine/AssetManager/AssetManager.h"

USanhwaAnimInstance::USanhwaAnimInstance()
{
	BS_LocomotionName = "BS_UE5MM_Locomotion";

	
}

bool USanhwaAnimInstance::IsAllResourceOK()
{
	return UMyGameAnimInstanceBase::IsAllResourceOK();
}


void USanhwaAnimInstance::UpdateAnimation(float dt)
{
	UMyGameAnimInstanceBase::UpdateAnimation(dt);


}

