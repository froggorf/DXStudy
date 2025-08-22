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

	void ChangeImage1();
	void ChangeImage2();

	void SetButton3Active(bool NewActive);
	void ChangeImage();
	
private:
	std::shared_ptr<FVerticalBoxWidget> ButtonVerticalBox;
	std::shared_ptr<FHorizontalBoxWidget> ButtonHorizontalBox;
	std::shared_ptr<FButtonWidget> TestButton1;
	std::shared_ptr<FButtonWidget> TestButton2;
	std::shared_ptr<FButtonWidget> TestButton3;

	std::shared_ptr<FImageWidget> TestImage1;
	std::shared_ptr<FImageWidget> TestImage2;

	std::shared_ptr<FTextWidget> Text1;
	std::shared_ptr<FTextWidget> ButtonText1;
	std::shared_ptr<FTextWidget> ButtonText2;
	std::shared_ptr<FTextWidget> ButtonText3;

};
