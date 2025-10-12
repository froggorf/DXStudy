#pragma once
#include "Engine/Timer/FTimerManager.h"
#include "Engine/Widget/UUserWidget.h"

class UNumberWidget : public UUserWidget
{
	MY_GENERATE_BODY(UNumberWidget)

	UNumberWidget();
	~UNumberWidget() override = default;

	void NativeConstruct() override;
public:
	void SetValue(UINT NewValue);
	void SetColor(const XMFLOAT4& NewTint);
	void SetHorizontalAlignment(EHorizontalAlignment NewAlignment);
	void SetDigitTexture(const std::shared_ptr<UTexture>& Texture);
	void SetDigitScale(float NewScale);
protected:
	std::shared_ptr<FHorizontalBoxWidget> HorBox;
		std::vector<std::shared_ptr<FImageWidget>> DigitImages;

private:
	UINT Length = 0;
	UINT Value = 0;

	std::shared_ptr<UTexture> DigitTexture;

	EHorizontalAlignment HorizontalAlignment = EHorizontalAlignment::Left;

	XMFLOAT4 Tint = {1,1,1,1};

	static constexpr float WidthPerDigit = 45.0f;
	static constexpr float HeightPerDigit = 60.0f;
	float DigitScale = 1.0f;

	static std::shared_ptr<UMaterialInterface> DigitMaterial;
};