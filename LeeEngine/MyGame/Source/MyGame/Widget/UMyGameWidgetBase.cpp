#include "CoreMinimal.h"
#include "UMyGameWidgetBase.h"


void UMyGameWidgetBase::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	OwnerWidget->SetDesignResolution({1720,980});

	AttackBoxSlot = std::make_shared<FHorizontalBoxWidget>();
	AttackBoxSlot->AttachToWidget(OwnerWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(AttackBoxSlot->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::RightBottom;
		CanvasSlot->Alignment = {1.0f,1.0f};
		CanvasSlot->Size = {300,150};
	}

	Skill_NativeConstruct();
	Ultimate_NativeConstruct();
}

void UMyGameWidgetBase::Skill_NativeConstruct()
{
	SkillBoxSlot = std::make_shared<FVerticalBoxWidget>();
	SkillBoxSlot->AttachToWidget(AttackBoxSlot);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(SkillBoxSlot->GetSlot()))
	{
		HBSlot->FillSize = 1.0f;
		HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
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
			static constexpr XMFLOAT2 SkillSize = {125,125};
			SkillBackground = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{1.0f,1.0f, 1.0f,0.5f}});
			SkillBackground->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillBackground->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}


			SkillUIImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache(GetSkillTextureName()), XMFLOAT4{1.0f,1.0f,1.0f,1.0f}});
			SkillUIImage->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(SkillUIImage->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			//FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{0.0f,0.0f,0.0f,0.9f}})
			PB_SkillCoolDown = std::make_shared<FProgressBarWidget>();
			PB_SkillCoolDown->AttachToWidget(SkillCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_SkillCoolDown->GetSlot()))
			{
				CanvasSlot->Size = SkillSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			PB_SkillCoolDown->SetVisibility(false);
			PB_SkillCoolDown->SetFillMode(EProgressBarFillMode::Radial_LeftToRight);
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
			SkillKeyText->SetText(L"E");
			SkillKeyText->SetFontSize(15.0f);
			SkillKeyText->SetFontColor({1.0f,1.0f,1.0f,1.0f});
			SkillKeyText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			SkillKeyText->SetVerticalAlignment(ETextVerticalAlignment::Center);
		}
	}
}

void UMyGameWidgetBase::Ultimate_NativeConstruct()
{
	UltimateBoxSlot = std::make_shared<FVerticalBoxWidget>();
	UltimateBoxSlot->AttachToWidget(AttackBoxSlot);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(UltimateBoxSlot->GetSlot()))
	{
		HBSlot->FillSize = 1.0f;
		HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
	}

	// ㄴ
	{
		UltimateCanvasWidget = std::make_shared<FCanvasWidget>();
		UltimateCanvasWidget->AttachToWidget(UltimateBoxSlot);
		if (const std::shared_ptr<FVerticalBoxSlot>& VBSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(UltimateCanvasWidget->GetSlot()))
		{
			VBSlot->FillSize = 4.f;
			VBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		// ㄴ
		{
			static constexpr XMFLOAT2 UltimateSize = {125,125};
			UltimateBackground = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{1.0f,1.0f, 1.0f,0.5f}});
			UltimateBackground->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateBackground->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}


			UltimateUIImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache(GetUltimateTextureName()), XMFLOAT4{1.0f,1.0f,1.0f,1.0f}});
			UltimateUIImage->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateUIImage->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize - XMFLOAT2{10,10};
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			PB_UltimateGauge = std::make_shared<FProgressBarWidget>();
			PB_UltimateGauge->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_UltimateGauge->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			PB_UltimateGauge->SetFillMode(EProgressBarFillMode::Radial_RightToLeft);
			PB_UltimateGauge->SetBackgroundImageBrush({UTexture::GetTextureCache("T_UltimateGauge"), {0.5f,0.5f,0.5f,0.5f}});
			PB_UltimateGauge->SetFillImageBrush({nullptr, UltimateGaugeColor});

			UltimateNonChargedImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{1.0f,1.0f, 1.0f,0.3f}});
			UltimateNonChargedImage->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateNonChargedImage->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize - XMFLOAT2{10,10};
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			PB_UltimateCoolDown = std::make_shared<FProgressBarWidget>();
			PB_UltimateCoolDown->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_UltimateCoolDown->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			PB_UltimateCoolDown->SetVisibility(false);
			PB_UltimateCoolDown->SetFillMode(EProgressBarFillMode::Radial_LeftToRight);
			PB_UltimateCoolDown->SetBackgroundImageBrush({UTexture::GetTextureCache("T_SkillBG"), XMFLOAT4{0.0f,0.0f,0.0f,0.9f}});
			PB_UltimateCoolDown->SetFillImageBrush({nullptr, XMFLOAT4{0.0f,0.0f,0.0f,0.0f}});

			UltimateCoolDownText = std::make_shared<FTextWidget>();
			UltimateCoolDownText->AttachToWidget(UltimateCanvasWidget);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateCoolDownText->GetSlot()))
			{
				CanvasSlot->Size = UltimateSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			UltimateCoolDownText->SetVisibility(false);
			UltimateCoolDownText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			UltimateCoolDownText->SetVerticalAlignment(ETextVerticalAlignment::Center);
			UltimateCoolDownText->SetFontSize(40.0f);
			UltimateCoolDownText->SetFontColor({1.0f,1.0f,1.0f,1.0f});
		}

		UltimateKeyGuideCanvas = std::make_shared<FCanvasWidget>();
		UltimateKeyGuideCanvas->AttachToWidget(UltimateBoxSlot);
		if (const std::shared_ptr<FVerticalBoxSlot>& VBSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(UltimateKeyGuideCanvas->GetSlot()))
		{
			VBSlot->FillSize = 1.0f;
			VBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		// ㄴ
		{
			static constexpr XMFLOAT2 GuideSize = {30,30};
			UltimateKeyGuideImage = std::make_shared<FImageWidget>(FImageBrush{ UTexture::GetTextureCache("T_KeyGuide"), XMFLOAT4{1.0f,1.0f,1.0f,0.5f}});
			UltimateKeyGuideImage->AttachToWidget(UltimateKeyGuideCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateKeyGuideImage->GetSlot()))
			{
				CanvasSlot->Size = GuideSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}

			UltimateKeyText = std::make_shared<FTextWidget>();
			UltimateKeyText->AttachToWidget(UltimateKeyGuideCanvas);
			if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(UltimateKeyText->GetSlot()))
			{
				CanvasSlot->Size = GuideSize;
				CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
				CanvasSlot->Alignment = {0.5f, 0.5f};
			}
			UltimateKeyText->SetText(L"Q");
			UltimateKeyText->SetFontSize(15.0f);
			UltimateKeyText->SetFontColor({1.0f,1.0f,1.0f,1.0f});
			UltimateKeyText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
			UltimateKeyText->SetVerticalAlignment(ETextVerticalAlignment::Center);
		}
	}
}

void UMyGameWidgetBase::SetSkillCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime)
{
	if (NewCoolDownTime <= 0.0f)
	{
		SkillCoolDownText->SetVisibility(false);
		PB_SkillCoolDown->SetVisibility(false);
		return;
	}

	SkillCoolDownText->SetVisibility(true);
	PB_SkillCoolDown->SetVisibility(true);
	PB_SkillCoolDown->SetValue(1 - NewCoolDownTime / MaxCoolDownTime);
	SkillCoolDownText->SetText(FloatToWString(NewCoolDownTime, 1));
}

void UMyGameWidgetBase::SetUltimateCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime)
{
	if (NewCoolDownTime <= 0.0f)
	{
		UltimateCoolDownText->SetVisibility(false);
		PB_UltimateCoolDown->SetVisibility(false);
		return;
	}

	UltimateCoolDownText->SetVisibility(true);
	PB_UltimateCoolDown->SetVisibility(true);
	PB_UltimateCoolDown->SetValue(1 - NewCoolDownTime / MaxCoolDownTime);
	UltimateCoolDownText->SetText(FloatToWString(NewCoolDownTime, 1));
}

void UMyGameWidgetBase::SetUltimateGauge(float NewGauge_0_To_1)
{
	PB_UltimateGauge->SetValue(NewGauge_0_To_1);
	UltimateNonChargedImage->SetVisibility(NewGauge_0_To_1 < 1.0f); 
}

void UMyGameWidgetBase::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);
}