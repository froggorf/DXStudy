#include "CoreMinimal.h"
#include "UMyTestWidget.h"

void UMyTestWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	OwnerWidget->SetDesignResolution({1720,1080});



	TestVerticalBox1 = std::make_shared<FVerticalBoxWidget>();
	TestVerticalBox1->AttachToWidget(OwnerWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(TestVerticalBox1->GetSlot()))
	{
		CanvasSlot->Position = {100,500};
		CanvasSlot->Size = {500,500};
	}

	TestImage1 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_Cube")}, XMFLOAT4{1,1,1,1});
	TestImage2 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White")}, XMFLOAT4{1,1,1,1});

	TestImage1->AttachToWidget(TestVerticalBox1);
	TestImage2->AttachToWidget(TestVerticalBox1);

	if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TestImage1->GetSlot()))
	{
		VerBoxSlot->FillSize = 1.0f;
	}

	if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TestImage2->GetSlot()))
	{
		VerBoxSlot->FillSize = 3.0f;
	}
}
