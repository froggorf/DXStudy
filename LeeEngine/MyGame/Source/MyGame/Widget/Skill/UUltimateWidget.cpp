#include "CoreMinimal.h"
#include "UUltimateWidget.h"

void UUltimateWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	MainCanvasWidget->SetDesignResolution({120, 150});

	SkillBoxSlot = std::make_shared<FVerticalBoxWidget>();
	SkillBoxSlot->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillBoxSlot->GetSlot()))
	{
		CanvasSlot->Anchors =ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}

	// ㄴ
	{
		SkillCanvasWidget = std::make_shared<FCanvasWidget>();
		SkillCanvasWidget->AttachToWidget(SkillBoxSlot);
		if (const std::shared_ptr<FVerticalBoxSlot>& VBSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(SkillCanvasWidget->GetSlot()))
		{
			VBSlot->FillSize = 4.f;
			VBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		// ㄴ
		{
			static constexpr XMFLOAT2 SkillSize = {120,120};
			SkillBackground = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{1.0f,1.0f, 1.0f,0.5f}});
			SkillBackground->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillBackground->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}


			SkillUIImage = std::make_shared<FImageWidget>(FImageBrush{ nullptr, XMFLOAT4{1.0f,1.0f,1.0f,1.0f}});
			SkillUIImage->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillUIImage->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			// 추가 부분
			PB_UltimateGauge = std::make_shared<FProgressBarWidget>();
			PB_UltimateGauge->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_UltimateGauge->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			PB_UltimateGauge->SetFillMode(EProgressBarFillMode::Radial_12To9);
			PB_UltimateGauge->SetBackgroundImageBrush({UTexture::GetTextureCache("T_UltimateGauge"), {0.5f,0.5f,0.5f,0.5f}});

			UltimateNonChargedImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{1.0f,1.0f, 1.0f,0.3f}});
			UltimateNonChargedImage->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateNonChargedImage->GetSlot()))
			{
				CanvasSlot->Size = SkillSize - XMFLOAT2{10,10};
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}


			PB_SkillCoolDown = std::make_shared<FProgressBarWidget>();
			PB_SkillCoolDown->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_SkillCoolDown->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			PB_SkillCoolDown->SetVisibility(false);
			PB_SkillCoolDown->SetFillMode(EProgressBarFillMode::Radial_12To3);
			PB_SkillCoolDown->SetBackgroundImageBrush({UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{0.0f,0.0f,0.0f,0.9f}});
			PB_SkillCoolDown->SetFillImageBrush({nullptr, XMFLOAT4{0.0f,0.0f,0.0f,0.0f}});

			SkillCoolDownText = std::make_shared<FTextWidget>();
			SkillCoolDownText->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillCoolDownText->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			SkillCoolDownText->SetVisibility(false);
			SkillCoolDownText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			SkillCoolDownText->SetVerticalAlignment(ETextVerticalAlignment::Center);
			SkillCoolDownText->SetFontSize(40.0f);
			SkillCoolDownText->SetFontColor({1.0f,1.0f,1.0f,1.0f});
		}

		SkillKeyGuideCanvas = std::make_shared<FCanvasWidget>();
		SkillKeyGuideCanvas->AttachToWidget(SkillBoxSlot);
		if (const std::shared_ptr<FVerticalBoxSlot>& VBSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(SkillKeyGuideCanvas->GetSlot()))
		{
			VBSlot->FillSize = 1.0f;
			VBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		// ㄴ
		{
			static constexpr XMFLOAT2 GuideSize = {30,30};
			SkillKeyGuideImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_KeyGuide"), XMFLOAT4{1.0f,1.0f,1.0f,0.5f}});
			SkillKeyGuideImage->AttachToWidget(SkillKeyGuideCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillKeyGuideImage->GetSlot()))
			{
				CanvasSlot->Size = GuideSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			SkillKeyText = std::make_shared<FTextWidget>();
			SkillKeyText->AttachToWidget(SkillKeyGuideCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillKeyText->GetSlot()))
			{
				CanvasSlot->Size = GuideSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			SkillKeyText->SetText(L"Q");
			SkillKeyText->SetFontSize(15.0f);
			SkillKeyText->SetFontColor({1.0f,1.0f,1.0f,1.0f});
			SkillKeyText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			SkillKeyText->SetVerticalAlignment(ETextVerticalAlignment::Center);
		}
	}

}

void UUltimateWidget::SetUltimateGaugeColor(const XMFLOAT4& NewColor)
{
	PB_UltimateGauge->SetFillImageBrush({nullptr, NewColor});
}

void UUltimateWidget::SetUltimateGauge(float NewGauge_0_To_1)
{
	PB_UltimateGauge->SetValue(NewGauge_0_To_1);
	UltimateNonChargedImage->SetVisibility(NewGauge_0_To_1 < 1.0f); 
}
