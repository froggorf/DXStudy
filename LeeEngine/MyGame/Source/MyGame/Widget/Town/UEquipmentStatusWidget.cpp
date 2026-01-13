#include "CoreMinimal.h"
#include "UEquipmentStatusWidget.h"
#include "MyGame/Core/UMyGameInstance.h"

void UEquipmentStatusWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	GoldBackground = std::make_shared<FImageWidget>(
		FImageBrush{ UTexture::GetTextureCache("T_White"), {0.2f, 0.2f, 0.2f, 0.9f} }
	);
	GoldBackground->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(GoldBackground->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftBottom;
		CanvasSlot->Alignment = { 0.0f, 1.0f };
		CanvasSlot->Position = { 20, -150 };
		CanvasSlot->Size = { 320, 48 };
	}

	GoldBox = std::make_shared<FHorizontalBoxWidget>();
	GoldBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(GoldBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::LeftBottom;
		CanvasSlot->Alignment = { 0.0f, 1.0f };
		CanvasSlot->Position = { 20, -150 };
		CanvasSlot->Size = { 320, 48 };
	}

	GoldIcon = std::make_shared<FImageWidget>(
		FImageBrush{ UTexture::GetTextureCache("T_Coin"), {1,1,1,1} }
	);
	GoldIcon->AttachToWidget(GoldBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(GoldIcon->GetSlot()))
	{
		HorBoxSlot->FillSize = 0.15f;
		HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		HorBoxSlot->SetHorizontalPadding({0, 0});
		HorBoxSlot->SetVerticalPadding({0, 0});
	}

	GoldText = std::make_shared<FTextWidget>();
	GoldText->SetText(L"0");
	GoldText->SetFontSize(26.0f);
	GoldText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});
	GoldText->SetHorizontalAlignment(ETextHorizontalAlignment::Left);
	GoldText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	GoldText->AttachToWidget(GoldBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(GoldText->GetSlot()))
	{
		HorBoxSlot->FillSize = 0.85f;
		HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		HorBoxSlot->SetHorizontalPadding({6, 0});
	}

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

		if (GoldText)
		{
			GoldText->SetText(std::to_wstring(GameInstance->GetGold()));
		}
	}
}
