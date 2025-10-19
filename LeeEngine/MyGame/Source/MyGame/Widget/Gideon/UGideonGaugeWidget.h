#pragma once
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

class UGideonGaugeWidget : public UUserWidget
{
	MY_GENERATE_BODY(UGideonGaugeWidget)

	UGideonGaugeWidget() = default;
	~UGideonGaugeWidget() override = default;

	void NativeConstruct() override;
	
	void SetLightningAttackChargeGauge(float ChargeTime, float MaxChargeTime);

private:
	static constexpr UINT GaugeSlotCount = 12;
	std::array<std::shared_ptr<FProgressBarWidget>,GaugeSlotCount> PB_GideonGauge;
	std::shared_ptr<FTextWidget> PercentText;
};
