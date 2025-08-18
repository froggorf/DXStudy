#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"
#include "Engine/World/UWorld.h"

class UMyTestWidget : public UUserWidget
{
	MY_GENERATE_BODY(UMyTestWidget)
	UMyTestWidget()
	{
		OwnerWidget = std::make_shared<FCanvasWidget>();
	}

	void NativeConstruct() override
	{
		OwnerWidget->SetDesignResolution({1720,1080});
		if (GEngine)
		{
			SetPlayerController(GEngine.get()->GetWorld().get()->GetPlayerController());
		}

		TestImage1 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_Cube")}, XMFLOAT4{1,1,1,1});
		TestImage2 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White")}, XMFLOAT4{1,1,1,1});

		TestImage1->AttachToWidget(OwnerWidget);
		TestImage2->AttachToWidget(OwnerWidget);

		if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(TestImage1->GetSlot()))
		{
			CanvasSlot->Position = {100, 100};
			CanvasSlot->Size = {100,100};
		}

		if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(TestImage2->GetSlot()))
		{
			CanvasSlot->Position = {500,500};
			CanvasSlot->Size = {100,100};
		}
	}

private:
	std::shared_ptr<FImageWidget> TestImage1;
	std::shared_ptr<FImageWidget> TestImage2;
};
