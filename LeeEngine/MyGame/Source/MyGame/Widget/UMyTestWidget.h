#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"
#include "Engine/World/UWorld.h"

class UMyTestWidget : public UUserWidget
{
	MY_GENERATE_BODY(UMyTestWidget)
	UMyTestWidget()
	{
		OwnerWidget = std::make_shared<FCanvasWidget>();
		OwnerWidget->SetDesignResolution({1920, 1080});
	}

	void NativeConstruct() override;
	void Tick(float DeltaSeconds) override;

	void SetValueTextButton(float NewValue);
private:
	std::shared_ptr<FImageWidget> MainBackgroundImage;
	std::shared_ptr<FTextWidget> ValueText;

	std::shared_ptr<FVerticalBoxWidget> VerticalBox;
	std::shared_ptr<FHorizontalBoxWidget> UpperHorBox;
	std::shared_ptr<FHorizontalBoxWidget> LowerHorBox;

	std::array<std::shared_ptr<FProgressBarWidget>, static_cast<UINT>(EProgressBarFillMode::Count)> ProgressBars;
	std::shared_ptr<FCanvasWidget> DummyCanvas;
	std::shared_ptr<FTextWidget> DummyText;

	std::shared_ptr<FButtonWidget> ValueUpButton;
	std::shared_ptr<FButtonWidget> ValueDownButton;

	std::shared_ptr<FTextWidget> ValueUpText;
	std::shared_ptr<FTextWidget> ValueDownText;

	float CurrentValue = 0.0f;
	float ValueChangeAxis = 0.0f;
};
