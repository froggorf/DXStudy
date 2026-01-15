#include "CoreMinimal.h"
#include "UStageLevelWidget.h"

#include "MyGame/Core/UMyGameInstance.h"

void UStageLevelWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	BorderImage = std::make_shared<FImageWidget>(
		FImageBrush{ UTexture::GetTextureCache("T_White"), {0.45f, 0.45f, 0.45f, 0.9f} }
	);
	BorderImage->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BorderImage->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftTop;
		CanvasSlot->Alignment = { 0.0f, 0.0f };
		CanvasSlot->Position = { 20, 20 };
		CanvasSlot->Size = { 240, 44 };
	}

	BackgroundImage = std::make_shared<FImageWidget>(
		FImageBrush{ UTexture::GetTextureCache("T_White"), {0.12f, 0.12f, 0.12f, 0.9f} }
	);
	BackgroundImage->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BackgroundImage->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftTop;
		CanvasSlot->Alignment = { 0.0f, 0.0f };
		CanvasSlot->Position = { 22, 22 };
		CanvasSlot->Size = { 236, 40 };
	}

	StageText = std::make_shared<FTextWidget>();
	StageText->SetFontSize(22.0f);
	StageText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});
	StageText->SetHorizontalAlignment(ETextHorizontalAlignment::Left);
	StageText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	StageText->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(StageText->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftTop;
		CanvasSlot->Alignment = { 0.0f, 0.0f };
		CanvasSlot->Position = { 30, 24 };
		CanvasSlot->Size = { 220, 36 };
	}

	UpdateStageLevel();
}

void UStageLevelWidget::UpdateStageLevel()
{
	UINT StageLevel = 1;
	if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
	{
		StageLevel = GameInstance->GetStageLevel();
	}

	if (StageText)
	{
		StageText->SetText(L"스테이지레벨 : " + std::to_wstring(StageLevel));
	}
}
