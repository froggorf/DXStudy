#pragma once
#include "Engine/Widget/UUserWidget.h"

class UHealthWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(UHealthWidgetBase)

	UHealthWidgetBase() = default;
	~UHealthWidgetBase() override = default;

	void NativeConstruct() override;
public:
	void SetHealthBarPercent(float Value, float MaxValue);
	void SetHealthBarPercent(float Value_0_To_1);
protected:
	std::shared_ptr<FProgressBarWidget> PB_HealthBar;
private:
};