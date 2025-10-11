#include "CoreMinimal.h"
#include "UPlayerHealthWidget.h"

void UPlayerHealthWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	//std::shared_ptr<FHorizontalBoxWidget> HealthHorBox;
	//std::shared_ptr<FTextWidget> HPText;
	//std::shared_ptr<FCanvasWidget> HealthCanvas;
	////		std::shared_ptr<FProgressBarWidget> PB_HealthBar << UHealthWidgetBase 에 있는 것, 계층을 나타내기 위해 주석
	//std::shared_ptr<FTextWidget> CurrentHealthText;

	HealthHorBox = std::make_shared<FHorizontalBoxWidget>();
	HealthHorBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(HealthHorBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}


	// ㄴ 계층
	{
		HPText = std::make_shared<FTextWidget>();
		HPText->AttachToWidget(HealthHorBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(HPText->GetSlot()))
		{
			HorBoxSlot->FillSize = 1.0f;
			HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}
		HPText->SetText(L"HP");
		HPText->SetHorizontalAlignment(ETextHorizontalAlignment::Left);
		HPText->SetVerticalAlignment(ETextVerticalAlignment::Center);
		HPText->SetFontColor({0.0f,0.0f,0.0f,1.0f});
		HPText->SetFontSize(20.0f);

		HealthCanvas = std::make_shared<FCanvasWidget>();
		HealthCanvas->AttachToWidget(HealthHorBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(HealthCanvas->GetSlot()))
		{
			HorBoxSlot->FillSize = 10.0f;
			HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		// ㄴ 계층
		{
			const std::shared_ptr<UTexture>& BaseTexture = UTexture::GetTextureCache("T_White");
			PB_DelayHealthBar = std::make_shared<FProgressBarWidget>();
			PB_DelayHealthBar->AttachToWidget(HealthCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_DelayHealthBar->GetSlot()))
			{
				CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
				CanvasSlot->Position = {0,0};
			}
			PB_DelayHealthBar->SetBackgroundImageBrush({BaseTexture, {1.0f,1.0f,1.0f,1.0f}});
			PB_DelayHealthBar->SetFillImageBrush({BaseTexture, {0.5f,0.5f,0.5f,1.0f}});
			PB_DelayHealthBar->SetFillMode(EProgressBarFillMode::LeftToRight);
			PB_DelayHealthBar->SetValue(1.0f);

			PB_HealthBar = std::make_shared<FProgressBarWidget>();
			PB_HealthBar->AttachToWidget(HealthCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_HealthBar->GetSlot()))
			{
				CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
				CanvasSlot->Position = {0,0};
			}
			PB_HealthBar->SetBackgroundImageBrush({nullptr, {0.5f,0.5f,0.5f,1.0f}});
			PB_HealthBar->SetFillImageBrush({BaseTexture, {0.0f,1.0f,0.0f,1.0f}});
			PB_HealthBar->SetFillMode(EProgressBarFillMode::LeftToRight);

			CurrentHealthText= std::make_shared<FTextWidget>();
			CurrentHealthText->AttachToWidget(HealthCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(CurrentHealthText->GetSlot()))
			{
				CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
				CanvasSlot->Position = {0,0};
			}
			CurrentHealthText->SetText(L"00000 / 00000");
			CurrentHealthText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			CurrentHealthText->SetVerticalAlignment(ETextVerticalAlignment::Center);
			CurrentHealthText->SetFontColor({0.0f,0.0f,0.0f,1.0f});
			CurrentHealthText->SetFontSize(20.0f);	
		}
	}
}

void UPlayerHealthWidget::SetHealthBarPercent(float Value, float MaxValue)
{
	UHealthWidgetBase::SetHealthBarPercent(Value , MaxValue);

	CurrentHealthText->SetText(FloatToWString(Value, 0) + L" / " + FloatToWString(MaxValue, 0));
}
