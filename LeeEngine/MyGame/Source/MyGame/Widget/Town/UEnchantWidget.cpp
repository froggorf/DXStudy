#include "CoreMinimal.h"
#include "UEnchantWidget.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Core/ATownGameMode.h"

void UEnchantWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	BaseImageWidget = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White"), {1,1,1,1}});
	BaseImageWidget->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BaseImageWidget->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.5f};
		CanvasSlot->Position = {0,0};
		CanvasSlot->Size = {1920/2, 1080/2};
	}

	HorBox= std::make_shared<FHorizontalBoxWidget>();
	HorBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(HorBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.5f};
		CanvasSlot->Position = {0,0};
		CanvasSlot->Size = {1920/2.5, 1080/2.5};
	}

	static std::string TextureName[static_cast<int>(EEquipType::Count)] = {"T_Armor_Head", "T_Armor_Body", "T_Armor_Glove", "T_Weapon"};
	static std::wstring EquipName[static_cast<int>(EEquipType::Count)] = {L"Head", L"Body", L"Glove", L"Weapon"};
	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		VerBox[i] = std::make_shared<FVerticalBoxWidget>();
		VerBox[i]->AttachToWidget(HorBox);
		if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(VerBox[i]->GetSlot()))
		{
			HorBoxSlot->FillSize = 1.0f;
			HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		}

		EquipImage[i] = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache(TextureName[i]), {1,1,1,1}});
		EquipImage[i]->AttachToWidget(VerBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(EquipImage[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 1.5f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			VerBoxSlot->SetHorizontalPadding({10,10});
			VerBoxSlot->SetVerticalPadding({10,10});
		}

		Dummy[i] = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache(""), {1,1,1,1}});
		Dummy[i]->AttachToWidget(VerBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(Dummy[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 0.5f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			VerBoxSlot->SetHorizontalPadding({10,10});
			VerBoxSlot->SetVerticalPadding({10,10});
		}

		EquipUpButton[i] = std::make_shared<FButtonWidget>();
		EquipUpButton[i]->SetStyle(EButtonType::Normal,FImageBrush{UTexture::GetTextureCache("T_White"), {0.8f,0.8f,0.8f,1.0f}});
		EquipUpButton[i]->SetStyle(EButtonType::Hovered,FImageBrush{UTexture::GetTextureCache("T_White"), {0.5f,0.5f,0.5f,1}});
		EquipUpButton[i]->SetStyle(EButtonType::Pressed,FImageBrush{UTexture::GetTextureCache("T_White"), {0.2f,0.2f,0.2f,1}});
		EquipUpButton[i]->AttachToWidget(VerBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(EquipUpButton[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 1.0f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			VerBoxSlot->SetHorizontalPadding({10,10});
			VerBoxSlot->SetVerticalPadding({10,10});
		}

		EquipText[i] = std::make_shared<FTextWidget>();
		EquipText[i]->SetText(EquipName[i]);
		EquipText[i]->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
		EquipText[i]->SetVerticalAlignment(ETextVerticalAlignment::Center);
		EquipText[i]->SetFontSize(30.0f);
		EquipText[i]->AttachToWidget(EquipUpButton[i]);
	}
	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		EquipUpButton[i]->OnClicked.Add([this, i]()
		{
			Enchant(static_cast<EEquipType>(i));
		});
	}
	
}

void UEnchantWidget::Enchant(EEquipType Type)
{
	if (const std::shared_ptr<ATownGameMode>& TownGameMode = std::dynamic_pointer_cast<ATownGameMode>(GetWorld()->GetPersistentLevel()->GetGameMode()))
	{
		TownGameMode->AddEquipLevel(Type);
	}
	
}

void UEnchantWidget::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);

	if (ImGui::IsKeyDown(ImGuiKey_Escape))
	{
		Close();
	}
}


void UEnchantWidget::Close()
{
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		PC->RemoveFromParent("EnchantWidget");
		GEngine->SetInputMode(EInputMode::InputMode_GameOnly);
		GEngine->SetMouseLock(EMouseLockMode::LockAlways);
		GEngine->ShowCursor(false);
	}
}
