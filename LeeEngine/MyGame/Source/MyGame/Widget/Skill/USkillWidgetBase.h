#pragma once
#include "Engine/Widget/UUserWidget.h"

class USkillWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(USkillWidgetBase)

	USkillWidgetBase() = default;
	~USkillWidgetBase() override = default;

	void NativeConstruct() override;
public:
	void SetSkillIconImageBrush(const FImageBrush& NewBrush);
	void SetSkillCoolDownTime(float NewCoolDownTime, float MaxCooldownTime);
protected:
	std::shared_ptr<FVerticalBoxWidget> SkillBoxSlot;
			std::shared_ptr<FCanvasWidget> SkillCanvasWidget;
				std::shared_ptr<FImageWidget> SkillBackground;
				std::shared_ptr<FImageWidget> SkillUIImage;
				std::shared_ptr<FProgressBarWidget> PB_SkillCoolDown;
				std::shared_ptr<FTextWidget> SkillCoolDownText;
			std::shared_ptr<FCanvasWidget> SkillKeyGuideCanvas;
				std::shared_ptr<FImageWidget> SkillKeyGuideImage;
				std::shared_ptr<FTextWidget> SkillKeyText;
private:
};