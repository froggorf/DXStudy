#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"

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

	// =========================== Attack ===========================
	std::shared_ptr<FHorizontalBoxWidget> AttackBoxSlot;

	// Skill UI
	std::shared_ptr<FVerticalBoxWidget> SkillBoxSlot;
	std::shared_ptr<FCanvasWidget> SkillCanvasWidget;
	std::shared_ptr<FImageWidget> SkillBackground;
	std::shared_ptr<FImageWidget> SkillUIImage;
	std::shared_ptr<FImageWidget> SkillCoolDownImage;
	std::shared_ptr<FTextWidget> SkillCoolDownText;
	std::shared_ptr<FCanvasWidget> SkillKeyGuideCanvas;
	std::shared_ptr<FImageWidget> SkillKeyGuideImage;
	std::shared_ptr<FTextWidget> SkillKeyText;
public:
	void SetSkillCoolDownTime(float NewCoolDownTime);
private:

	// Ultimate UI
	std::shared_ptr<FVerticalBoxWidget> UltimateBoxSlot;
	// ===============================================================
};
