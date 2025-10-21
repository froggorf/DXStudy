#pragma once
#include "MyGame/Component/Combat/Melee/USanhwaCombatComponent.h"
#include "MyGame/Widget/UMyGameWidgetBase.h"

class UGideonGaugeWidget;

class UGideonWidget : public UMyGameWidgetBase
{
	MY_GENERATE_BODY(UGideonWidget)

	UGideonWidget() = default;
	~UGideonWidget() override = default;

	void NativeConstruct() override;
	void SetLightningAttackChargeGauge(float ChargeTime, float MaxChargeTime);

	// TODO: 수정하기
	std::string GetSkillTextureName() override  { return "T_Icon_Sanhwa_Skill";}
	std::string GetBasicAttackTextureName() override { return "T_Icon_Sanhwa_BasicAttack";}
	std::string GetUltimateTextureName() override { return "T_Icon_Sanhwa_Ultimate";}

	void SetAimModeWidgetVisibility(bool NewVisible) const;
private:
	std::shared_ptr<FTextWidget> CrossHairText;
	std::shared_ptr<UGideonGaugeWidget> GideonGaugeWidget;
};
