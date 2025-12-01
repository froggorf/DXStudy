#include "CoreMinimal.h"
#include "AGameMode.h"

void AGameMode::Register()
{
	AActor::Register();

	InitGame();
}
