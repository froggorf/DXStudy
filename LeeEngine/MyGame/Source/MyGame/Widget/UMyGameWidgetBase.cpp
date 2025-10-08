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

	// Skill
	// ㄴ
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

	

	UltimateBoxSlot = std::make_shared<FVerticalBoxWidget>();
	UltimateBoxSlot->AttachToWidget(AttackBoxSlot);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(UltimateBoxSlot->GetSlot()))
	{
		HBSlot->FillSize = 1.0f;
		HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
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

void UMyGameWidgetBase::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);
}


//
//void UMyGameWidgetBase::NativeConstruct()
//{
//	UUserWidget::NativeConstruct();
//
//	OwnerWidget->SetDesignResolution({1720,980});
//
//	MainBackgroundImage = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White"), XMFLOAT4{0.5f,0.5f,0.5f,1.0f}});
//	MainBackgroundImage->AttachToWidget(OwnerWidget);
//	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(MainBackgroundImage->GetSlot()))
//	{
//		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
//		CanvasSlot->Alignment = {0.5f,0.5f};
//		CanvasSlot->Size = {1200,600};
//	}
//
//	ValueText = std::make_shared<FTextWidget>();
//	CurrentValue = 0.0f;
//	SetValueTextButton(0.0f);
//	ValueText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
//	ValueText->SetVerticalAlignment(ETextVerticalAlignment::Top);
//	ValueText->SetFontSize(40.0f);
//	ValueText->AttachToWidget(OwnerWidget);
//	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(ValueText->GetSlot()))
//	{
//		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
//		CanvasSlot->Alignment = {0.5f,0.5f};
//		CanvasSlot->Position = {0.0f,-200.0f};
//		CanvasSlot->Size = {500,150};
//	}
//
//	VerticalBox = std::make_shared<FVerticalBoxWidget>();
//	VerticalBox->AttachToWidget(OwnerWidget);
//	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(VerticalBox->GetSlot()))
//	{
//		CanvasSlot->Anchors = ECanvasAnchor::LeftBottom;
//		CanvasSlot->Alignment = {0.5f,-0.5f};
//		CanvasSlot->Position = {600.0f,-700.0f};
//		CanvasSlot->Size = {600,200};
//	}
//
//	UpperHorBox = std::make_shared<FHorizontalBoxWidget>();
//	UpperHorBox->AttachToWidget(VerticalBox);
//	LowerHorBox = std::make_shared<FHorizontalBoxWidget>();
//	LowerHorBox->AttachToWidget(VerticalBox);
//	if (const std::shared_ptr<FVerticalBoxSlot>& BoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(UpperHorBox->GetSlot()))
//	{
//		BoxSlot->FillSize = 1.0f;
//	}
//	if (const std::shared_ptr<FVerticalBoxSlot>& BoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(LowerHorBox->GetSlot()))
//	{
//		BoxSlot->FillSize = 1.0f;
//	}
//
//	for (UINT i = 0; i < 4; ++i)
//	{
//		ProgressBars[i] = std::make_shared<FProgressBarWidget>();
//		ProgressBars[i]->SetValue(CurrentValue);
//		ProgressBars[i]->AttachToWidget(UpperHorBox);
//		ProgressBars[i]->SetFillMode(static_cast<EProgressBarFillMode>(i));
//		if (const std::shared_ptr<FHorizontalBoxSlot>& BoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(ProgressBars[i]->GetSlot()))
//		{
//			BoxSlot->FillSize = 1.0f;
//			BoxSlot->SetVerticalPadding({10,10});
//			BoxSlot->SetHorizontalPadding({10,10});
//		}
//	}
//	for (UINT i = 4; i < static_cast<UINT>(EProgressBarFillMode::Count); ++i)
//	{
//		ProgressBars[i] = std::make_shared<FProgressBarWidget>();
//		ProgressBars[i]->SetValue(CurrentValue);
//		ProgressBars[i]->AttachToWidget(LowerHorBox);
//		ProgressBars[i]->SetFillMode(static_cast<EProgressBarFillMode>(i));
//		if (const std::shared_ptr<FHorizontalBoxSlot>& BoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(ProgressBars[i]->GetSlot()))
//		{
//			BoxSlot->FillSize = 1.0f;
//			BoxSlot->SetVerticalPadding({10,10});
//			BoxSlot->SetHorizontalPadding({10,10});
//		}
//	}
//	DummyCanvas = std::make_shared<FCanvasWidget>();
//	DummyCanvas->AttachToWidget(LowerHorBox);
//	if (const std::shared_ptr<FHorizontalBoxSlot>& BoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(DummyCanvas->GetSlot()))
//	{
//		BoxSlot->FillSize = 1.0f;
//	}
//
//	// TODO 해당 부분 잘 안나오는거 같으니 나중에 확인하기
//	//DummyText = std::make_shared<FTextWidget>();
//	//DummyText->AttachToWidget(DummyCanvas);
//	//DummyText->SetText(L"빈칸");
//	//DummyText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
//	//DummyText->SetVerticalAlignment(ETextVerticalAlignment::Center);
//	//DummyText->SetFontSize(30.0f);
//	//if (const std::shared_ptr<FCanvasSlot>& BoxSlot = std::dynamic_pointer_cast<FCanvasSlot>(DummyText->GetSlot()))
//	//{
//	//	BoxSlot->Anchors = ECanvasAnchor::WrapAll;
//	//	BoxSlot->Position = {0,0};
//	//	BoxSlot->Size = {100,100};
//	//}
//
//	ValueUpButton = std::make_shared<FButtonWidget>();
//	ValueUpButton->AttachToWidget(OwnerWidget);
//	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(ValueUpButton->GetSlot()))
//	{
//		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
//		CanvasSlot->Alignment = {0.5f,0.5f};
//		CanvasSlot->Position = {150, -100};
//		CanvasSlot->Size = {200,100};
//	}
//	ValueUpButton->OnPressed.Add([this]()
//	{
//		this->ValueChangeAxis = (0.1f);
//	});
//	ValueUpButton->OnReleased.Add([this]()
//	{
//		this->ValueChangeAxis = 0.0f;
//	});
//
//	ValueDownButton = std::make_shared<FButtonWidget>();
//	ValueDownButton->AttachToWidget(OwnerWidget);
//	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(ValueDownButton->GetSlot()))
//	{
//		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
//		CanvasSlot->Alignment = {0.5f,0.5f};
//		CanvasSlot->Position = {375, -100};
//		CanvasSlot->Size = {200,100};
//	}
//	ValueDownButton->OnPressed.Add([this]()
//		{
//			this->ValueChangeAxis = (-0.1f);
//		});
//	ValueDownButton->OnReleased.Add([this]()
//		{
//			this->ValueChangeAxis = 0.0f;
//		});
//	ValueUpText = std::make_shared<FTextWidget>();
//	ValueUpText->SetText(L"값 올리기");
//	ValueUpText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
//	ValueUpText->SetVerticalAlignment(ETextVerticalAlignment::Center);
//	ValueUpText->SetFontSize(30.0f);
//	ValueUpText->AttachToWidget(ValueUpButton);
//
//	ValueDownText = std::make_shared<FTextWidget>();
//	ValueDownText ->SetText(L"값 내리기");
//	ValueDownText ->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
//	ValueDownText ->SetVerticalAlignment(ETextVerticalAlignment::Center);
//	ValueDownText ->SetFontSize(30.0f);
//	ValueDownText ->AttachToWidget(ValueDownButton);
//}
//
//void UMyGameWidgetBase::Tick(float DeltaSeconds)
//{
//	UUserWidget::Tick(DeltaSeconds);
//
//	if (std::abs(ValueChangeAxis) > FLT_EPSILON)
//	{
//		if (ValueChangeAxis < 0.0f)
//		{
//			SetValueTextButton(-DeltaSeconds);	
//		}
//		else
//		{
//			SetValueTextButton(DeltaSeconds);
//		}
//		
//	}
//}
//
//void UMyGameWidgetBase::SetValueTextButton(float NewAddValue)
//{
//	CurrentValue += NewAddValue;
//	if (ValueText)
//	{
//		ValueText->SetText(L"현재 값 : " + std::to_wstring(CurrentValue));	
//	}
//	for (UINT i = 0; i < static_cast<UINT>(EProgressBarFillMode::Count); ++i)
//	{
//		if (ProgressBars[i])
//		{
//			ProgressBars[i]->SetValue(CurrentValue);	
//		}
//	}
//}