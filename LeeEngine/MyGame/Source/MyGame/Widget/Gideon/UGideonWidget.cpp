#include "CoreMinimal.h"
#include "UGideonWidget.h"

void UGideonWidget::NativeConstruct()
{
	UMyGameWidgetBase::NativeConstruct();

	CrossHairText = std::make_shared<FTextWidget>();
	CrossHairText->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(CrossHairText->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f,0.5f};
		CanvasSlot->Size = {300,300};
	}
	CrossHairText->SetText(L"+");
	CrossHairText->SetFontColor({1,0,0,1});
	CrossHairText->SetFontSize(50.0f);
	CrossHairText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	CrossHairText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	CrossHairText->SetVisibility(false);
}

void UGideonWidget::SetCrossHairVisibility(bool NewVisible) const
{
	if (CrossHairText)
	{
		CrossHairText->SetVisibility(NewVisible);
	}
}
