#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"

class UNumberWidget;
class UPlayerHealthWidget;
class UUltimateWidget;
class USkillWidgetBase;

class UMyGameWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(UMyGameWidgetBase)
	UMyGameWidgetBase() = default;
	~UMyGameWidgetBase() override = default;

	void NativeConstruct() override;

	virtual std::string GetBasicAttackTextureName() { return ""; }
	virtual std::string GetSkillTextureName() { return ""; }
	virtual std::string GetUltimateTextureName() {return ""; }
private:
	// =========================== Health ===========================
	std::shared_ptr<FCanvasWidget> HealthCanvas;
		std::shared_ptr<UPlayerHealthWidget> HealthWidget;

	// =========================== Attack ===========================
	std::shared_ptr<FHorizontalBoxWidget> AttackBoxSlot;
		// Skill UI UserWidget
		std::shared_ptr<USkillWidgetBase> SkillCoolDownWidget;
		// Ultimate UI UserWidget
		std::shared_ptr<UUltimateWidget> UltimateCoolDownWidget;

public:
	void SetSkillCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime);
	void SetUltimateCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime);
	void SetUltimateGauge(float NewGauge_0_To_1);
	void SetHealthBarWidget(float CurValue, float MaxValue);
	void Tick(float DeltaSeconds) override;

protected:
	XMFLOAT4 UltimateGaugeColor = {0,1,1,1};
};
