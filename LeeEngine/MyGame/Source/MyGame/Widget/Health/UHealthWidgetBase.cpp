#include "CoreMinimal.h"
#include "UHealthWidgetBase.h"

#include "Engine/RenderCore/EditorScene.h"

#include <cmath>

UHealthWidgetBase::UHealthWidgetBase()
{
	MainCanvasWidget->SetDesignResolution({300.0f,25.0f});
}

void UHealthWidgetBase::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	const std::shared_ptr<UTexture>& BaseTexture = UTexture::GetTextureCache("T_White");
	PB_DelayHealthBar = std::make_shared<FProgressBarWidget>();
	PB_DelayHealthBar->AttachToWidget(MainCanvasWidget);
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
	PB_HealthBar->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_HealthBar->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}
	PB_HealthBar->SetBackgroundImageBrush({nullptr, {0.5f,0.5f,0.5f,1.0f}});
	PB_HealthBar->SetFillImageBrush({BaseTexture, {0.0f,1.0f,0.0f,1.0f}});
	PB_HealthBar->SetFillMode(EProgressBarFillMode::LeftToRight);

}

void UHealthWidgetBase::SetHealthBarPercent(float Value, float MaxValue)
{
	if (MaxValue == 0.0f)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "MaxValue == 0.0f");
		return;
	}

	if (!std::isfinite(Value) || !std::isfinite(MaxValue))
	{
		float Percent = Value > 0.0f ? 1.0f : 0.0f;
		PB_HealthBar->SetValue(Percent);
	}
	else
	{
		float Percent = std::clamp(Value/MaxValue, 0.0f, 1.0f);
		PB_HealthBar->SetValue(Percent);
	}

	GEngine->GetTimerManager()->SetTimer(DelayHealthTimerHandle, {this, &UHealthWidgetBase::SetDelayHealthBar}, 0.0f, true, DelayTimerRepeatTime);
	CurrentDelayTime = 0.0f;
	DelayState = EHealthBarDelayState::Wait;
	StartedDelayHealth = PB_DelayHealthBar->GetValue();
}

void UHealthWidgetBase::SetDelayHealthBar()
{
	CurrentDelayTime += DelayTimerRepeatTime;

	switch (DelayState)
	{
	case EHealthBarDelayState::Wait:
		if (CurrentDelayTime >= DelayWaitTime)
		{
			CurrentDelayTime = 0.0f;
			DelayState = EHealthBarDelayState::Reduce;
		}
		break;
	case EHealthBarDelayState::Reduce:
		{
			float DelayApplyTimeNotZero = DelayAppliedTime == 0.0f ? 0.5f : DelayAppliedTime;
			if (CurrentDelayTime > DelayApplyTimeNotZero)
			{
				PB_DelayHealthBar->SetValue(PB_HealthBar->GetValue());
				GEngine->GetTimerManager()->ClearTimer(DelayHealthTimerHandle);
			}
			else
			{
				PB_DelayHealthBar->SetValue(std::lerp(StartedDelayHealth, PB_HealthBar->GetValue(), CurrentDelayTime/DelayApplyTimeNotZero));	
			}
			
			
		}
		break;
	default:
		return;
	}
}
