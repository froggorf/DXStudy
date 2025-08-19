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
	}

	void NativeConstruct() override;

private:
	std::shared_ptr<FVerticalBoxWidget> TestVerticalBox1;

	std::shared_ptr<FImageWidget> TestImage1;
	std::shared_ptr<FImageWidget> TestImage2;
};
