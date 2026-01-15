#pragma once

#include "Engine/Widget/UUserWidget.h"

class UStageLevelWidget : public UUserWidget
{
	MY_GENERATE_BODY(UStageLevelWidget)

public:
	UStageLevelWidget() = default;
	~UStageLevelWidget() override = default;

	void NativeConstruct() override;
	void UpdateStageLevel();

private:
	std::shared_ptr<class FImageWidget> BorderImage;
	std::shared_ptr<class FImageWidget> BackgroundImage;
	std::shared_ptr<class FTextWidget> StageText;
};
