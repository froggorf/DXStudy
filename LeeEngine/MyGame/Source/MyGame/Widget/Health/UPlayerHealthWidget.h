#pragma once
#include "UHealthWidgetBase.h"

class UPlayerHealthWidget : public UHealthWidgetBase
{
	MY_GENERATE_BODY(UPlayerHealthWidget)

	UPlayerHealthWidget() = default;
	~UPlayerHealthWidget() override = default;

	void NativeConstruct() override;
public:
	void SetHealthBarPercent(float Value, float MaxValue) override;
protected:
	std::shared_ptr<FHorizontalBoxWidget> HealthHorBox;
		std::shared_ptr<FTextWidget> HPText;
		std::shared_ptr<FCanvasWidget> HealthCanvas;
	//		std::shared_ptr<FProgressBarWidget> PB_HealthBar << UHealthWidgetBase 에 있는 것, 계층을 나타내기 위해 주석
			std::shared_ptr<FTextWidget> CurrentHealthText;
private:

};