#include "CoreMinimal.h"
#include "UHealthWidgetBase.h"

#include "Engine/RenderCore/EditorScene.h"

void UHealthWidgetBase::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	PB_HealthBar = std::make_shared<FProgressBarWidget>();
	PB_HealthBar->SetBackgroundImageBrush({UTexture::GetTextureCache("T_White"), {0.5f,0.5f,0.5f,1.0f}});
	PB_HealthBar->SetFillImageBrush({nullptr, {0.0f,1.0f,0.0f,1.0f}});
	PB_HealthBar->SetFillMode(EProgressBarFillMode::LeftToRight);
}

void UHealthWidgetBase::SetHealthBarPercent(float Value, float MaxValue)
{
	if (MaxValue == 0.0f)
	{
		MY_LOG(GetFunctionName, EDebugLogLevel::DLL_Error, "MaxValue == 0.0f");
		return;
	}

	float Percent = std::clamp(Value/MaxValue, 0.0f, 1.0f);
	PB_HealthBar->SetValue(Percent);
}

void UHealthWidgetBase::SetHealthBarPercent(float Value_0_To_1)
{
	PB_HealthBar->SetValue(std::clamp(Value_0_To_1, 0.0f,1.0f));
}
