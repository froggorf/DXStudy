#pragma once
#include "Engine/Timer/FTimerManager.h"
#include "Engine/Widget/UUserWidget.h"

enum class EHealthBarDelayState
{
	None,
	Wait,
	Reduce,
};

class UHealthWidgetBase : public UUserWidget
{
	MY_GENERATE_BODY(UHealthWidgetBase)

	UHealthWidgetBase() = default;
	~UHealthWidgetBase() override = default;

	void NativeConstruct() override;
public:
	virtual void SetHealthBarPercent(float Value, float MaxValue);

	void SetDelayHealthBar();
protected:
	std::shared_ptr<FProgressBarWidget> PB_HealthBar;
	std::shared_ptr<FProgressBarWidget> PB_DelayHealthBar;
private:
	float StartedDelayHealth = 0.0f;
	float DelayWaitTime = 0.5f;
	// 나눠지는 값이므로 절대 0이 되어선 안됨
	static constexpr float DelayAppliedTime = 0.5f;
	EHealthBarDelayState DelayState = EHealthBarDelayState::None;
	float CurrentDelayTime = 0.0f;
	FTimerHandle DelayHealthTimerHandle;
	static constexpr float DelayTimerRepeatTime = 0.01f;
};