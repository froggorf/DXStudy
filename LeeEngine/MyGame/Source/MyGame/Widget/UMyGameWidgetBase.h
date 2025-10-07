#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"

class UMyGameWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(UMyGameWidgetBase)
	UMyGameWidgetBase() = default;
	~UMyGameWidgetBase() override = default;

	void NativeConstruct() override;
private:

	// =========================== Attack ===========================
	std::shared_ptr<FHorizontalBoxWidget> AttackBoxSlot;

	// Skill UI
	std::shared_ptr<FVerticalBoxWidget> SkillBoxSlot;
	std::shared_ptr<FImageWidget> SkillUIImage;
	std::shared_ptr<FTextWidget> SkillCoolDownText;

	// Ultimate UI
	std::shared_ptr<FVerticalBoxWidget> UltimateBoxSlot;
	// ===============================================================
};
