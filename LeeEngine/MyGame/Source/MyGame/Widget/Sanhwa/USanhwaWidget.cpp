#include "CoreMinimal.h"
#include "USanhwaWidget.h"

void USanhwaWidget::NativeConstruct()
{
	UMyGameWidgetBase::NativeConstruct();


	GaugeCanvas = std::make_shared<FCanvasWidget>();
	GaugeCanvas->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(GaugeCanvas->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterBottom;
		CanvasSlot->Alignment = {0.5f, 1.0f};
		CanvasSlot->Position = {0, -200};
		CanvasSlot->Size = {600, 50};
	}

	GaugeHBBox = std::make_shared<FHorizontalBoxWidget>();
	GaugeHBBox->AttachToWidget(GaugeCanvas);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(GaugeHBBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Alignment = {0.0f,0.0f};
		CanvasSlot->Position = {0, 0};
		CanvasSlot->Size = {100,100};
	}

	const std::shared_ptr<UTexture>& Tex = UTexture::GetTextureCache("T_White");
	for (UINT i = 0; i < USanhwaCombatComponent::GaugeSize; ++i)
	{
		PB_Gauge[i] = std::make_shared<FProgressBarWidget>();
		PB_Gauge[i]->AttachToWidget(GaugeHBBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(PB_Gauge[i]->GetSlot()))
		{
			HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			HBSlot->FillSize = 1.0f;
		}
		PB_Gauge[i]->SetValue(0.0f);
		
		PB_Gauge[i]->SetBackgroundImageBrush({Tex, {0.5f,0.5f,0.5f,1.0f}});
		PB_Gauge[i]->SetFillImageBrush({Tex, {(145.0f/255.0f), (186.0f / 255.0f), (191.0f / 255.0f), 1.0f}});
	}

	PB_ChargeGauge = std::make_shared<FProgressBarWidget>();
	PB_ChargeGauge->AttachToWidget(GaugeCanvas);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_ChargeGauge->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Alignment = {0.0f,0.0f};
		CanvasSlot->Position = {0, 0};
	}
	PB_ChargeGauge->SetBackgroundImageBrush({nullptr, {0,0,0,1}});
	PB_ChargeGauge->SetFillImageBrush({nullptr, {0,0,1,1}});
	PB_ChargeGauge->SetSlider({Tex, {1,1,1,1}}, {5, 75});
	PB_ChargeGauge->SetVisibility(false);
}

void USanhwaWidget::Tick(float DeltaSeconds)
{
	UMyGameWidgetBase::Tick(DeltaSeconds);
}

void USanhwaWidget::SetGaugeUI(const std::array<bool, USanhwaCombatComponent::GaugeSize>& CurrentGauge)
{
	for (UINT i = 0; i < USanhwaCombatComponent::GaugeSize; ++i)
	{
		float NewValue = CurrentGauge[i] ? 1.0f : 0.0f;
		PB_Gauge[i]->SetValue(NewValue);
	}
}