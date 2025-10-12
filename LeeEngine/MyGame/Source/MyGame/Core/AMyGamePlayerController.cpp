#include "CoreMinimal.h"
#include "AMyGamePlayerController.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Actor/Damage/AFloatingDamageActor.h"

void AMyGamePlayerController::SpawnFloatingDamage(const FTransform& SpawnTransform, const XMFLOAT4& Color, UINT Value, float DigitScale, const std::shared_ptr<UTexture>& DigitTexture)
{
	// TODO: 나중엔 오브젝트 풀링
	if (std::shared_ptr<AFloatingDamageActor> FloatingDamageActor 
				= std::dynamic_pointer_cast<AFloatingDamageActor>(GetWorld()->GetPersistentLevel()->SpawnActor("AFloatingDamageActor", SpawnTransform)))
	{
		FloatingDamageActor->Setting(Color, Value, DigitScale, DigitTexture);	
	}

}
