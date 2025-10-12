#include "CoreMinimal.h"
#include "UMyGameWidgetBase.h"

#include "Health/UPlayerHealthWidget.h"
#include "Number/UNumberWidget.h"
#include "Skill/USkillWidgetBase.h"
#include "Skill/UUltimateWidget.h"


void UMyGameWidgetBase::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	MainCanvasWidget->SetDesignResolution({1720,980});

	HealthCanvas = std::make_shared<FCanvasWidget>();
	HealthCanvas->AttachToWidget(MainCanvasWidget);

	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(HealthCanvas->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterBottom;
		CanvasSlot->Alignment = {0.5f, 1.0f};
		CanvasSlot->Position = {0, 0};
		CanvasSlot->Size = {600, 50};
	}
	HealthWidget = std::make_shared<UPlayerHealthWidget>();
	HealthWidget->AttachToPanel(HealthCanvas);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(HealthWidget->GetMainCanvasWidget()->GetSlot()))
	{
		CanvasSlot->Anchors =ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}


	AttackBoxSlot = std::make_shared<FHorizontalBoxWidget>();
	AttackBoxSlot->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(AttackBoxSlot->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::RightBottom;
		CanvasSlot->Alignment = {1.0f,1.0f};
		CanvasSlot->Size = {300,150};
	}

	SkillCoolDownWidget = std::make_shared<USkillWidgetBase>();
	SkillCoolDownWidget->AttachToPanel(AttackBoxSlot);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(SkillCoolDownWidget->GetMainCanvasWidget()->GetSlot()))
	{
		HBSlot->FillSize = 1.0f;
		HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
	}
	SkillCoolDownWidget->SetSkillIconImageBrush(FImageBrush{ UTexture::GetTextureCache(GetSkillTextureName()), XMFLOAT4{1.0f,1.0f,1.0f,1.0f}});


	UltimateCoolDownWidget = std::make_shared<UUltimateWidget>();
	UltimateCoolDownWidget->AttachToPanel(AttackBoxSlot);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HBSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(UltimateCoolDownWidget->GetMainCanvasWidget()->GetSlot()))
	{
		HBSlot->FillSize = 1.0f;
		HBSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HBSlot->VerticalAlignment = EVerticalAlignment::Wrap;
	}
	UltimateCoolDownWidget->SetSkillIconImageBrush(FImageBrush{UTexture::GetTextureCache(GetUltimateTextureName()), XMFLOAT4{1.0f,1.0f,1.0f, 1.0f}});
	UltimateCoolDownWidget->SetUltimateGaugeColor(UltimateGaugeColor);

}


void UMyGameWidgetBase::SetSkillCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime)
{
	if (SkillCoolDownWidget)
	{
		SkillCoolDownWidget->SetSkillCoolDownTime(NewCoolDownTime, MaxCoolDownTime);
	}
}

void UMyGameWidgetBase::SetUltimateCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime)
{
	UltimateCoolDownWidget->SetSkillCoolDownTime(NewCoolDownTime, MaxCoolDownTime);
}

void UMyGameWidgetBase::SetUltimateGauge(float NewGauge_0_To_1)
{
	UltimateCoolDownWidget->SetUltimateGauge(NewGauge_0_To_1);
}

void UMyGameWidgetBase::SetHealthBarWidget(float CurValue, float MaxValue)
{
	if (HealthWidget)
	{
		HealthWidget->SetHealthBarPercent(CurValue,MaxValue);
	}
}

void UMyGameWidgetBase::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);

}
