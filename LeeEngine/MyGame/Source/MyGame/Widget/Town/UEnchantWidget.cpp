#include "CoreMinimal.h"
#include "UEnchantWidget.h"

#include "Engine/World/UWorld.h"
#include "Engine/FAudioDevice.h"
#include "MyGame/Core/ATownGameMode.h"
#include "MyGame/Core/UMyGameInstance.h"


static std::wstring GetUpgradeResultMessage(EEquipUpgradeResult Result)
{
	switch (Result)
	{
	case EEquipUpgradeResult::InvalidUser:
		return L"Invalid user.";
	case EEquipUpgradeResult::NoData:
		return L"Upgrade data missing.";
	case EEquipUpgradeResult::NotEnoughGold:
		return L"Not enough gold.";
	case EEquipUpgradeResult::DbError:
		return L"DB error.";
	default:
		return L"";
	}
}

void UEnchantWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	constexpr float BaseWidth = 1920.0f / 2.0f;
	constexpr float BaseHeight = 1080.0f / 2.0f;
	constexpr float ClosePadding = 10.0f;
	constexpr float CloseSize = 32.0f;

	BaseImageWidget = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White"), {1,1,1,1}});
	BaseImageWidget->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BaseImageWidget->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.5f};
		CanvasSlot->Position = {0,0};
		CanvasSlot->Size = {BaseWidth, BaseHeight};
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

	CloseButton = std::make_shared<FButtonWidget>();
	CloseButton->SetStyle(EButtonType::Normal, FImageBrush{UTexture::GetTextureCache("T_White"), {0.85f, 0.1f, 0.1f, 1.0f}});
	CloseButton->SetStyle(EButtonType::Hovered, FImageBrush{UTexture::GetTextureCache("T_White"), {1.0f, 0.2f, 0.2f, 1.0f}});
	CloseButton->SetStyle(EButtonType::Pressed, FImageBrush{UTexture::GetTextureCache("T_White"), {0.6f, 0.05f, 0.05f, 1.0f}});
	CloseButton->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(CloseButton->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {1.0f, 0.0f};
		CanvasSlot->Position = {BaseWidth * 0.5f - ClosePadding, -BaseHeight * 0.5f + ClosePadding};
		CanvasSlot->Size = {CloseSize, CloseSize};
	}

	CloseButtonText = std::make_shared<FTextWidget>();
	CloseButtonText->SetText(L"X");
	CloseButtonText->SetFontSize(22.0f);
	CloseButtonText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});
	CloseButtonText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	CloseButtonText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	CloseButtonText->AttachToWidget(CloseButton);

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

		EquipCostText[i] = std::make_shared<FTextWidget>();
		EquipCostText[i]->SetText(L"Cost: ?");
		EquipCostText[i]->SetFontSize(24.0f);
		EquipCostText[i]->SetFontColor({0.0f, 0.0f, 0.0f, 1.0f});
		EquipCostText[i]->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
		EquipCostText[i]->SetVerticalAlignment(ETextVerticalAlignment::Center);
		EquipCostText[i]->AttachToWidget(VerBox[i]);
		if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(EquipCostText[i]->GetSlot()))
		{
			VerBoxSlot->FillSize = 0.6f;
			VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
			VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
			VerBoxSlot->SetHorizontalPadding({10,10});
			VerBoxSlot->SetVerticalPadding({0,10});
		}
	}
	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		EquipUpButton[i]->OnClicked.Add([this, i]()
		{
			Enchant(static_cast<EEquipType>(i));
		});
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.Add([this]()
		{
			Close();
		});
	}

	FeedbackText = std::make_shared<FTextWidget>();
	FeedbackText->SetText(L"");
	FeedbackText->SetFontSize(28.0f);
	FeedbackText->SetFontColor({1.0f, 0.2f, 0.2f, 1.0f});
	FeedbackText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	FeedbackText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	FeedbackText->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(FeedbackText->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterBottom;
		CanvasSlot->Alignment = {0.5f, 1.0f};
		CanvasSlot->Position = {0, -160};
		CanvasSlot->Size = {600, 50};
	}

	UpdateUpgradeCostTexts();
	
}

void UEnchantWidget::Enchant(EEquipType Type)
{
	PlaySound2DByName("SB_SFX_UI_Click_01");
	if (const std::shared_ptr<ATownGameMode>& TownGameMode = std::dynamic_pointer_cast<ATownGameMode>(GetWorld()->GetPersistentLevel()->GetGameMode()))
	{
		const EEquipUpgradeResult Result = TownGameMode->AddEquipLevel(Type);
		if (Result == EEquipUpgradeResult::Success)
		{
			PlaySound2DByName("SB_SFX_UI_Confirm");
			ShowFeedback(L"", {0.0f, 0.0f, 0.0f, 0.0f});
			UpdateUpgradeCostTexts();
		}
		else
		{
			PlaySound2DByName("SB_SFX_UI_Error");
			ShowFeedback(GetUpgradeResultMessage(Result), {1.0f, 0.2f, 0.2f, 1.0f});
		}
	}
	else
	{
		PlaySound2DByName("SB_SFX_UI_Error");
		ShowFeedback(L"Upgrade failed.", {1.0f, 0.2f, 0.2f, 1.0f});
	}
	
}

void UEnchantWidget::UpdateUpgradeCostTexts()
{
	UMyGameInstance* GameInstance = UMyGameInstance::GetInstance<UMyGameInstance>();
	if (!GameInstance)
	{
		return;
	}

	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		if (!EquipCostText[i])
		{
			continue;
		}

		int Cost = 0;
		if (GameInstance->GetEquipUpgradeCost(static_cast<EEquipType>(i), Cost))
		{
			EquipCostText[i]->SetText(L"Cost: " + std::to_wstring(Cost));
		}
		else
		{
			EquipCostText[i]->SetText(L"Cost: ?");
		}
	}
}

void UEnchantWidget::ShowFeedback(const std::wstring& Message, const XMFLOAT4& Color)
{
	if (!FeedbackText)
	{
		return;
	}

	FeedbackText->SetText(Message);
	FeedbackText->SetFontColor(Color);
	FeedbackRemainTime = Message.empty() ? 0.0f : 2.0f;
}

void UEnchantWidget::Tick(float DeltaSeconds)
{
	UUserWidget::Tick(DeltaSeconds);

	if (FeedbackRemainTime > 0.0f)
	{
		FeedbackRemainTime -= DeltaSeconds;
		if (FeedbackRemainTime <= 0.0f && FeedbackText)
		{
			FeedbackText->SetText(L"");
		}
	}
}


void UEnchantWidget::Close()
{
	PlaySound2DByName("SB_SFX_UI_Click_01");
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		PC->RemoveFromParent("EnchantWidget");
		GEngine->SetInputMode(EInputMode::InputMode_GameOnly);
		GEngine->SetMouseLock(EMouseLockMode::LockAlways);
		GEngine->ShowCursor(false);
	}
}
