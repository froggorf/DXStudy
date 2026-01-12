#include "CoreMinimal.h"
#include "UEquipmentStatusWidget.h"
#include "MyGame/Core/UMyGameInstance.h"

void UEquipmentStatusWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	BackgroundImage = std::make_shared<FImageWidget>(
		FImageBrush{ UTexture::GetTextureCache("T_White"), {0.2f, 0.2f, 0.2f, 0.9f} }
	);
	BackgroundImage->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BackgroundImage->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftBottom;
		CanvasSlot->Alignment = { 0.0f, 1.0f };
		CanvasSlot->Position = { 20, -20 };
		CanvasSlot->Size = { 320, 120 };
	}

	EquipmentSlotsBox = std::make_shared<FHorizontalBoxWidget>();
	EquipmentSlotsBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(EquipmentSlotsBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftBottom;
		CanvasSlot->Alignment = { 0.0f, 1.0f };
		CanvasSlot->Position = { 20, -40 };
		CanvasSlot->Size = { 320, 80 };
	}

	static std::string TextureNames[static_cast<int>(EEquipType::Count)] = { 
		"T_Armor_Head", "T_Armor_Body", "T_Armor_Glove", "T_Weapon" 
	};

	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		SlotVerticalBox[i] = std::make_shared<FVerticalBoxWidget>();
		SlotVerticalBox[i]->AttachToWidget(EquipmentSlotsBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(SlotVerticalBox[i]->GetSlot()))
		{
			HorBoxSlot->FillSize = 1.0f;
			HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			HorBoxSlot->SetHorizontalPadding({5, 5});
		}

		EquipImage[i] = std::make_shared<FImageWidget>(
			FImageBrush{ UTexture::GetTextureCache(TextureNames[i]), {1,1,1,1} }
		);
		EquipImage[i]->AttachToWidget(SlotVerticalBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(EquipImage[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 1.0f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		

		EquipLevelText[i] = std::make_shared<FTextWidget>();
		EquipLevelText[i]->SetText(L"Lv. ?");
		EquipLevelText[i]->SetFontSize(30.0f);
		EquipLevelText[i]->SetFontColor({0.0f, 0.0f, 0.0f, 1.0f});
		EquipLevelText[i]->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
		EquipLevelText[i]->SetVerticalAlignment(ETextVerticalAlignment::Center);
		EquipLevelText[i]->AttachToWidget(SlotVerticalBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(EquipLevelText[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 0.4f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}
	}


	UpdateEquipmentData();
}

void UEquipmentStatusWidget::UpdateEquipmentData()
{
	if (UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>())
	{
		const auto& EquipLevels = GameInstance->GetEquipLevel();

		for(int i = 0; i < 4; ++i)
		{
			if(EquipLevelText[i])
			{
				EquipLevelText[i]->SetText(L"Lv. " + std::to_wstring(EquipLevels[i]));
			}
		}
	}
}
