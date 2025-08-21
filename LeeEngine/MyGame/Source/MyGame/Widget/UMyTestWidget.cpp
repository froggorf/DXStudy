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

	TestText1 = std::make_shared<FTextWidget>();
	TestText1->AttachToWidget(OwnerWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(TestText1->GetSlot()))
	{
		CanvasSlot->Position = {700,500};
		CanvasSlot->Size = {500,500};
	}
	TestText1->SetText(L"이건 Canvas에 부착한 Text");
	TestText1->SetFontSize(50.0f);

	TestText2 = std::make_shared<FTextWidget>();
	TestText2->AttachToWidget(TestVerticalBox1);
	if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TestText2->GetSlot()))
	{
		VerBoxSlot->FillSize = 1.0f;
	}
	TestText2->SetText(L"이건 VerticalBox에 부착한 Text");
	TestText2->SetFontSize(50.0f);
}
