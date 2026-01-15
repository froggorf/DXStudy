#include "CoreMinimal.h"
#include "UNumberWidget.h"

#include "Engine/Material/UMaterial.h"

std::shared_ptr<UMaterialInterface> UNumberWidget::DigitMaterial = nullptr;

UNumberWidget::UNumberWidget()
{
}

void UNumberWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	if (!DigitTexture)
	{
		DigitTexture = UTexture::GetTextureCache("T_Number");
	}

	HorBox = std::make_shared<FHorizontalBoxWidget>();
	HorBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(HorBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::WrapAll;
		CanvasSlot->Position = {0,0};
	}
}

void UNumberWidget::SetValue(UINT NewValue)
{
	if (NewValue == Value && Length > 0)
	{
		return;
	}

	if (!DigitMaterial)
	{
		DigitMaterial = UMaterial::GetMaterialCache("M_NumberWidget");
	}

	std::string ValueText = std::to_string(NewValue);
	Length = static_cast<UINT>(ValueText.size());
	Value = NewValue;

	MainCanvasWidget->SetDesignResolution({ Length * WidthPerDigit * DigitScale , HeightPerDigit * DigitScale });

	HorBox->ClearChildren();
	UINT CurDigitImageSize = static_cast<UINT>(DigitImages.size());
	if (CurDigitImageSize < Length)
	{
		for (UINT i = 0; i < Length - CurDigitImageSize; ++i)
		{
			std::shared_ptr<FImageWidget> NewWidget = std::make_shared<FImageWidget>(FImageBrush{DigitTexture, Tint});
			NewWidget->SetOverrideMaterial(DigitMaterial);
			DigitImages.emplace_back(NewWidget);
		}
	}
	else
	{
		DigitImages.resize(Length);
	}

	SetColor(Tint);
	SetDigitTexture(DigitTexture);

	for (UINT i = 0; i < Length; ++i)
	{
		FSystemParamConstantBuffer SV;
		SV.Int_1 = ValueText[i] - '0';
		DigitImages[i]->SetSystemParam(SV);
		DigitImages[i]->AttachToWidget(HorBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(DigitImages[i]->GetSlot()))
		{
			HorBoxSlot->FillSize = 1.0f;
			HorBoxSlot->HorizontalAlignment = HorizontalAlignment;
			HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}
	}
}

void UNumberWidget::SetColor(const XMFLOAT4& NewTint)
{
	Tint = NewTint;
	for (UINT i = 0; i < Length; ++i)
	{
		DigitImages[i]->SetColor(NewTint);
	}
}

void UNumberWidget::SetHorizontalAlignment(EHorizontalAlignment NewAlignment)
{
	HorizontalAlignment = NewAlignment;
	for (UINT i = 0; i < Length; ++i)
	{
		if (const std::shared_ptr<FHorizontalBoxSlot>& Slot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(DigitImages[i]->GetSlot()))
		{
			Slot->HorizontalAlignment = HorizontalAlignment;
		} 
	}
}

void UNumberWidget::SetDigitScale(float NewScale)
{
	DigitScale = NewScale;
	MainCanvasWidget->SetDesignResolution({ Length * WidthPerDigit * DigitScale , HeightPerDigit * DigitScale });
}



void UNumberWidget::SetDigitTexture(const std::shared_ptr<UTexture>& Texture)
{
	if (Texture == nullptr)
	{
		return;
	}

	DigitTexture = Texture;
	for (UINT i = 0; i < Length; ++i)
	{
		DigitImages[i]->SetTexture(DigitTexture);
	}

}
