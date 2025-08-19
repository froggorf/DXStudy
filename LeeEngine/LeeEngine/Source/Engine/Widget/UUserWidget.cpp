#include "CoreMinimal.h"
#include "UUserWidget.h"

#include "Engine/UEngine.h"
#include "Engine/World/UWorld.h"

void UUserWidget::NativeConstruct()
{
	if (GEngine)
	{
		SetPlayerController(GEngine.get()->GetWorld().get()->GetPlayerController());
	}
}
