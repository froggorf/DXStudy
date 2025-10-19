#include "CoreMinimal.h"
#include "UGideonGaugeWidget.h"

void UGideonGaugeWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	static_assert(GaugeSlotCount > 1);

	// 뒤쪽 인덱스일 수록 뒤쪽에 그려짐
	const std::shared_ptr<UTexture> T_GaugeTexture = UTexture::GetTextureCache("T_GideonGauge");
	for (int i = static_cast<int>(GaugeSlotCount) - 1; i >= 0; --i)
	{
		PB_GideonGauge[i] = std::make_shared<FProgressBarWidget>();
		PB_GideonGauge[i]->SetFillMode(EProgressBarFillMode::Radial_9To12);
		PB_GideonGauge[i]->AttachToWidget(MainCanvasWidget);
		if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PB_GideonGauge[i]->GetSlot()))
		{
			CanvasSlot->Anchors=ECanvasAnchor::WrapAll;
			CanvasSlot->Position = {0,0};
		}

		// 마지막 인덱스(GaugeSlotCount-1) 의 경우엔 BackBrush도 그려야함
		const XMFLOAT4 BackgroundColor = i == GaugeSlotCount - 1 ? XMFLOAT4{0.5f,0.5f,0.5f,0.75f} : XMFLOAT4{0.0f,0.0f,0.0f,0.0f};
		PB_GideonGauge[i]->SetBackgroundImageBrush(FImageBrush{ T_GaugeTexture, BackgroundColor});
		float LerpValue = static_cast<float>(i) / (GaugeSlotCount - 1);
		float RedVal = std::lerp(0.0f, 2.0f, LerpValue);
		float GreenVal = std::lerp(2.0f,0.0f, LerpValue);
		PB_GideonGauge[i]->SetFillImageBrush(FImageBrush{nullptr, XMFLOAT4{RedVal, GreenVal, 0.0f, 1.0f}});
		PB_GideonGauge[i]->SetValue(0.0f);
	}

	PercentText	 = std::make_shared<FTextWidget>();
	PercentText->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(PercentText->GetSlot()))
	{
		CanvasSlot->Anchors=ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f,0.5f};
		CanvasSlot->Size = {200, 100};
	}
	PercentText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	PercentText->SetVerticalAlignment(ETextVerticalAlignment::Bottom);
	PercentText->SetText(L"0 %");
	PercentText->SetFontSize(45.0f);
	PercentText->SetFontColor({0,1,0,1});
}

void UGideonGaugeWidget::SetLightningAttackChargeGauge(float ChargeTime, float MaxChargeTime)
{
	assert(!(MaxChargeTime < FLT_EPSILON));
	
	// 0~0.5를 채워야함
	float Value = (ChargeTime / MaxChargeTime) * 0.5f;
	// 한 칸당 수치
	float Gap = 0.5f / GaugeSlotCount;
	for (UINT i = 0; i < GaugeSlotCount; ++i)
	{
		PB_GideonGauge[i]->SetValue(0.0f);

		float CurSlotStartValue = static_cast<float>(i)*Gap;
		float CurSlotEndValue = static_cast<float>(i+1)*Gap;
		// 그릴 필요 없음
		if (CurSlotStartValue > Value)
		{
			continue;
		}

		PB_GideonGauge[i]->SetValue(std::min(CurSlotEndValue, Value));
	}

	float RedVal = std::lerp(0.0f, 2.0f, Value*2);
	float GreenVal = std::lerp(2.0f,0.0f, Value*2);
	PercentText->SetFontColor({RedVal, GreenVal, 0.0f, 1.0f});

	std::wstring ValueText = FloatToWString(Value * 100 * 2, 0);
	// . 지우기
	ValueText.erase(ValueText.end()-1);
	PercentText->SetText(ValueText + L" %");
}
