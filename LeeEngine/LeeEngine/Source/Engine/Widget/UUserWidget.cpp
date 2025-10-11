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

	for (const std::shared_ptr<UUserWidget>& AttachedUserWidget : AttachedUserWidgets)
	{
		AttachedUserWidget->Tick_AttachedUserWidgetVersion(DeltaSeconds);
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

void UUserWidget::AttachToCanvas(const std::shared_ptr<FCanvasWidget>& ParentCanvas)
{
	if (!MainCanvasWidget || !ParentCanvas)
	{
		return;
	}

	// 먼저 ParentCanvas의 루트의 Tick을 돌려서 위치를 갱신해주고
	const std::shared_ptr<FPanelWidget>& ParentRoot = ParentCanvas->GetRootWidget();
	ParentRoot->Tick(0.0f);

	MainCanvasWidget->AttachToWidget(ParentCanvas);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(MainCanvasWidget->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}

	const std::shared_ptr<FSlot>& ParentSlot = ParentCanvas->GetSlot();
	XMFLOAT2 NewDesignResolution = {ParentSlot->GetRight() - ParentSlot->GetLeft(), ParentSlot->GetBottom() - ParentSlot->GetTop()};
	MainCanvasWidget->SetDesignResolution(NewDesignResolution);

	if (UUserWidget* OwnerUserWidget = ParentRoot->GetOwnerUserWidget())
	{
		OwnerUserWidget->AttachedUserWidgets.emplace_back(shared_from_this());
	}

	NativeConstruct();
}
