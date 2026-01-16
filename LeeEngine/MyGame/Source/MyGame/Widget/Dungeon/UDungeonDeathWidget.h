#pragma once

#include "Engine/Widget/UUserWidget.h"

class UDungeonDeathWidget : public UUserWidget
{
	MY_GENERATE_BODY(UDungeonDeathWidget)

public:
	UDungeonDeathWidget() = default;
	~UDungeonDeathWidget() override = default;

	void NativeConstruct() override;

private:
	void ChangeLevel(const std::string& LevelName);

	std::shared_ptr<class FImageWidget> BackgroundImage;
	std::shared_ptr<class FVerticalBoxWidget> MainBox;
	std::shared_ptr<class FTextWidget> TitleText;
	std::shared_ptr<class FHorizontalBoxWidget> ButtonBox;
	std::shared_ptr<class FButtonWidget> RetryButton;
	std::shared_ptr<class FTextWidget> RetryButtonText;
	std::shared_ptr<class FButtonWidget> TownButton;
	std::shared_ptr<class FTextWidget> TownButtonText;
};
