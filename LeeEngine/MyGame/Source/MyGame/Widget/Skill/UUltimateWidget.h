#pragma once
#include "USkillWidgetBase.h"

class UUltimateWidget : public USkillWidgetBase
{
	MY_GENERATE_BODY(UUltimateWidget)

	UUltimateWidget() = default;
	~UUltimateWidget() override = default;

	// Ult 의 경우 Skill 에서 일부 추가되는 것들이 존재하여 별도로 NativeConstruct 를 오버라이딩함
	void NativeConstruct() override;
public:
	void SetUltimateGaugeColor(const XMFLOAT4& NewColor);
	void SetUltimateGauge(float NewGauge_0_To_1);
protected:
	// ㄴ SkillBoxSlot
	std::shared_ptr<FProgressBarWidget> PB_UltimateGauge;
	std::shared_ptr<FImageWidget> UltimateNonChargedImage;

private:
};