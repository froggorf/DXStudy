#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"

class UMyGameWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(UMyGameWidgetBase)
	UMyGameWidgetBase() = default;
	~UMyGameWidgetBase() override = default;

	void NativeConstruct() override;

	void Skill_NativeConstruct();
	void Ultimate_NativeConstruct();

	virtual std::string GetBasicAttackTextureName() { return ""; }
	virtual std::string GetSkillTextureName() { return ""; }
	virtual std::string GetUltimateTextureName() {return ""; }
private:

	// =========================== Attack ===========================
	std::shared_ptr<FHorizontalBoxWidget> AttackBoxSlot;
	// Skill UI
		std::shared_ptr<FVerticalBoxWidget> SkillBoxSlot;
			std::shared_ptr<FCanvasWidget> SkillCanvasWidget;
				std::shared_ptr<FImageWidget> SkillBackground;
				std::shared_ptr<FImageWidget> SkillUIImage;
				std::shared_ptr<FProgressBarWidget> PB_SkillCoolDown;
				std::shared_ptr<FTextWidget> SkillCoolDownText;
			std::shared_ptr<FCanvasWidget> SkillKeyGuideCanvas;
				std::shared_ptr<FImageWidget> SkillKeyGuideImage;
				std::shared_ptr<FTextWidget> SkillKeyText;

	// Ultimate UI
		std::shared_ptr<FVerticalBoxWidget> UltimateBoxSlot;
			std::shared_ptr<FCanvasWidget> UltimateCanvasWidget;
				std::shared_ptr<FImageWidget> UltimateBackground;
				std::shared_ptr<FImageWidget> UltimateUIImage;
				std::shared_ptr<FProgressBarWidget> PB_UltimateGauge;
				std::shared_ptr<FImageWidget> UltimateNonChargedImage;
				std::shared_ptr<FProgressBarWidget> PB_UltimateCoolDown;
				std::shared_ptr<FTextWidget> UltimateCoolDownText;
			std::shared_ptr<FCanvasWidget> UltimateKeyGuideCanvas;
				std::shared_ptr<FImageWidget> UltimateKeyGuideImage;
				std::shared_ptr<FTextWidget> UltimateKeyText;
public:
	void SetSkillCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime);
	void SetUltimateCoolDownTime(float NewCoolDownTime, float MaxCoolDownTime);
	void SetUltimateGauge(float NewGauge_0_To_1);
	void Tick(float DeltaSeconds) override;

protected:
	XMFLOAT4 UltimateGaugeColor = {0,1,1,1};
};
