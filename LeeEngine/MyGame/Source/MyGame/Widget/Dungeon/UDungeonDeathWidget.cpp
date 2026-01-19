#include "CoreMinimal.h"
#include "UDungeonDeathWidget.h"

#include "Engine/World/UWorld.h"
#include "Engine/FAudioDevice.h"


void UDungeonDeathWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	constexpr float BaseWidth = 600.0f;
	constexpr float BaseHeight = 190.0f;

	BackgroundImage = std::make_shared<FImageWidget>(
		FImageBrush{UTexture::GetTextureCache("T_White"), {0.05f, 0.05f, 0.05f, 0.85f}}
	);
	BackgroundImage->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(BackgroundImage->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.5f};
		CanvasSlot->Position = {0, 0};
		CanvasSlot->Size = {BaseWidth, BaseHeight};
	}

	MainBox = std::make_shared<FVerticalBoxWidget>();
	MainBox->AttachToWidget(MainCanvasWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(MainBox->GetSlot()))
	{
		CanvasSlot->Anchors = ECanvasAnchor::CenterMiddle;
		CanvasSlot->Alignment = {0.5f, 0.5f};
		CanvasSlot->Position = {0, 0};
		CanvasSlot->Size = {BaseWidth, BaseHeight};
	}

	TitleText = std::make_shared<FTextWidget>();
	TitleText->SetText(L"사망하였습니다.");
	TitleText->SetFontSize(42.0f);
	TitleText->SetFontColor({1.0f, 0.2f, 0.2f, 1.0f});
	TitleText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	TitleText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	TitleText->AttachToWidget(MainBox);
	if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TitleText->GetSlot()))
	{
		VerBoxSlot->FillSize = 1.2f;
		VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		VerBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
	}

	ButtonBox = std::make_shared<FHorizontalBoxWidget>();
	ButtonBox->AttachToWidget(MainBox);
	if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(ButtonBox->GetSlot()))
	{
		VerBoxSlot->FillSize = 1.0f;
		VerBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		VerBoxSlot->VerticalAlignment = EVerticalAlignment::Middle;
	}

	RetryButton = std::make_shared<FButtonWidget>();
	RetryButton->SetStyle(EButtonType::Normal, FImageBrush{UTexture::GetTextureCache("T_White"), {0.2f, 0.6f, 0.2f, 1.0f}});
	RetryButton->SetStyle(EButtonType::Hovered, FImageBrush{UTexture::GetTextureCache("T_White"), {0.3f, 0.7f, 0.3f, 1.0f}});
	RetryButton->SetStyle(EButtonType::Pressed, FImageBrush{UTexture::GetTextureCache("T_White"), {0.1f, 0.5f, 0.1f, 1.0f}});
	RetryButton->AttachToWidget(ButtonBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(RetryButton->GetSlot()))
	{
		HorBoxSlot->FillSize = 1.0f;
		HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		HorBoxSlot->SetHorizontalPadding({10, 10});
	}

	RetryButtonText = std::make_shared<FTextWidget>();
	RetryButtonText->SetText(L"Retry");
	RetryButtonText->SetFontSize(26.0f);
	RetryButtonText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});
	RetryButtonText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	RetryButtonText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	RetryButtonText->AttachToWidget(RetryButton);

	TownButton = std::make_shared<FButtonWidget>();
	TownButton->SetStyle(EButtonType::Normal, FImageBrush{UTexture::GetTextureCache("T_White"), {0.2f, 0.4f, 0.8f, 1.0f}});
	TownButton->SetStyle(EButtonType::Hovered, FImageBrush{UTexture::GetTextureCache("T_White"), {0.3f, 0.5f, 0.9f, 1.0f}});
	TownButton->SetStyle(EButtonType::Pressed, FImageBrush{UTexture::GetTextureCache("T_White"), {0.1f, 0.3f, 0.7f, 1.0f}});
	TownButton->AttachToWidget(ButtonBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorBoxSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(TownButton->GetSlot()))
	{
		HorBoxSlot->FillSize = 1.0f;
		HorBoxSlot->HorizontalAlignment = EHorizontalAlignment::Wrap;
		HorBoxSlot->VerticalAlignment = EVerticalAlignment::Wrap;
		HorBoxSlot->SetHorizontalPadding({10, 10});
	}

	TownButtonText = std::make_shared<FTextWidget>();
	TownButtonText->SetText(L"Town");
	TownButtonText->SetFontSize(26.0f);
	TownButtonText->SetFontColor({1.0f, 1.0f, 1.0f, 1.0f});
	TownButtonText->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	TownButtonText->SetVerticalAlignment(ETextVerticalAlignment::Center);
	TownButtonText->AttachToWidget(TownButton);

	RetryButton->OnClicked.Add([this]()
		{
			ChangeLevel("DungeonLevel");
		});

	TownButton->OnClicked.Add([this]()
		{
			ChangeLevel("TownLevel");
		});
}

void UDungeonDeathWidget::ChangeLevel(const std::string& LevelName)
{
	PlaySound2DByName("SB_SFX_UI_Click_01");
	if (APlayerController* PC = GetWorld()->GetPlayerController())
	{
		PC->RemoveFromParent("DungeonDeath");
	}

	GEngine->SetInputMode(EInputMode::InputMode_GameOnly);
	GEngine->SetMouseLock(EMouseLockMode::LockAlways);
	GEngine->ShowCursor(false);
	GEngine->ChangeLevelByName(LevelName);
}
