#pragma once
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

class USanhwaWidget : public UMyGameWidgetBase
{
	MY_GENERATE_BODY(USanhwaWidget)

	USanhwaWidget() = default;
	~USanhwaWidget() override = default;

	void NativeConstruct() override;

	void Tick(float DeltaSeconds) override;

	std::string GetSkillTextureName() override  { return "T_Icon_Sanhwa_Skill";}
	std::string GetBasicAttackTextureName() override { return "T_Icon_Sanhwa_BasicAttack";}
	std::string GetUltimateTextureName() override { return "T_Icon_Sanhwa_Ultimate";}

	void SetGaugeUI(const std::array<bool, USanhwaCombatComponent::GaugeSize>& CurrentGauge);
	const std::shared_ptr<FProgressBarWidget>& GetChargeProgressBar() const {return PB_ChargeGauge;}
private:
	// ======================== Gauge ========================
	std::shared_ptr<FCanvasWidget> GaugeCanvas;
		std::shared_ptr<FHorizontalBoxWidget> GaugeHBBox;
			std::shared_ptr<FProgressBarWidget> PB_Gauge[USanhwaCombatComponent::GaugeSize];
		std::shared_ptr<FProgressBarWidget> PB_ChargeGauge;
		
};
