#include "CoreMinimal.h"
#include "UUserWidget.h"

#include "Engine/World/UWorld.h"

void UUserWidget::Tick(float DeltaSeconds)
{
	if (OwnerWidget)
	{
		OwnerWidget->Tick(DeltaSeconds);
	}
}

void UUserWidget::NativeConstruct()
{
	if (GEngine)
	{
		SetPlayerController(GEngine.get()->GetCurrentWorld().get()->GetPlayerController());
	}
}

void UUserWidget::CollectAllWidgets(std::vector<std::shared_ptr<FChildWidget>>& Widgets)
{
	if (OwnerWidget)
	{
		OwnerWidget->CollectAllWidgets(Widgets);
	}
}
