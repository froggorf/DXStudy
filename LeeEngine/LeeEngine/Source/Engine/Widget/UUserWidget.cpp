#include "CoreMinimal.h"
#include "UUserWidget.h"

#include "Engine/World/UWorld.h"

UUserWidget::UUserWidget()
{
	MainCanvasWidget = std::make_shared<FCanvasWidget>();
	MainCanvasWidget->SetOwnerUserWidget(this);
	MainCanvasWidget->SetDesignResolution({1920, 1080});
}

void UUserWidget::Tick(float DeltaSeconds)
{
	if (MainCanvasWidget)
	{
		MainCanvasWidget->Tick(DeltaSeconds);
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
	if (MainCanvasWidget)
	{
		MainCanvasWidget->CollectAllWidgets(Widgets);
	}
}

void UUserWidget::AttachToPanel(const std::shared_ptr<FPanelWidget>& ParentPanelWidget)
{
	if (!MainCanvasWidget || !ParentPanelWidget)
	{
		return;
	}

	// 먼저 ParentCanvas의 루트의 Tick을 돌려서 위치를 갱신해주고
	const std::shared_ptr<FPanelWidget>& ParentRoot = ParentPanelWidget->GetRootWidget();
	ParentRoot->Tick(0.0f);

	// 해당 UserWidget을 ParentPanelWidget에 부착
	MainCanvasWidget->AttachToWidget(ParentPanelWidget);
	ParentPanelWidget->AttachedUserWidget.emplace_back(shared_from_this());

	NativeConstruct();
}
