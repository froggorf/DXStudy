#include "CoreMinimal.h"
#include "UMyTestWidget.h"

void UMyTestWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	OwnerWidget->SetDesignResolution({1720,880});

	ButtonVerticalBox = std::make_shared<FVerticalBoxWidget>();
	ButtonVerticalBox->AttachToWidget(OwnerWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(ButtonVerticalBox->GetSlot()))
	{
		CanvasSlot->Position = {50,300};
		CanvasSlot->Size = {800,150};
	}

	TestImage1 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_CircleEffect")}, XMFLOAT4{1.0f,1.0f,1.0f,1.0f});
	TestImage1->AttachToWidget(OwnerWidget);
	if (const std::shared_ptr<FCanvasSlot>& CanvasSlot = std::dynamic_pointer_cast<FCanvasSlot>(TestImage1->GetSlot()))
	{
		CanvasSlot->Position = {900,200};
		CanvasSlot->Size = {500,500};
	}

	Text1 = std::make_shared<FTextWidget>();
	Text1->SetText(L"버튼들");
	Text1->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	Text1->SetVerticalAlignment(ETextVerticalAlignment::Center);
	Text1->SetFontSize(25.0f);
	Text1->AttachToWidget(ButtonVerticalBox);
	if (const std::shared_ptr<FVerticalBoxSlot>& VerticalSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(Text1->GetSlot()))
	{
		VerticalSlot->FillSize = 1.0f;
	}

	ButtonHorizontalBox = std::make_shared<FHorizontalBoxWidget>();
	ButtonHorizontalBox->AttachToWidget(ButtonVerticalBox);
	if (const std::shared_ptr<FVerticalBoxSlot>& VerticalSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(ButtonHorizontalBox ->GetSlot()))
	{
		VerticalSlot->FillSize = 3.0f;
	}


	TestButton1 = std::make_shared<FButtonWidget>();
	TestButton2 = std::make_shared<FButtonWidget>();
	TestButton3 = std::make_shared<FButtonWidget>();
	TestButton1->AttachToWidget(ButtonHorizontalBox);
	TestButton2->SetStyle(EButtonType::Normal, {UTexture::GetTextureCache("T_Cube"), XMFLOAT4{0.7f,0.7f,0.7f,1.0f}});
	TestButton3->SetDisabled(true);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorizontalSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(TestButton1->GetSlot()))
	{
		HorizontalSlot->FillSize = 1.0f;
	}
	TestButton2->AttachToWidget(ButtonHorizontalBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorizontalSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(TestButton2->GetSlot()))
	{
		HorizontalSlot->FillSize = 1.0f;
	}
	TestButton3->AttachToWidget(ButtonHorizontalBox);
	if (const std::shared_ptr<FHorizontalBoxSlot>& HorizontalSlot = std::dynamic_pointer_cast<FHorizontalBoxSlot>(TestButton3->GetSlot()))
	{
		HorizontalSlot->FillSize = 1.0f;
	}

	ButtonText1 = std::make_shared<FTextWidget>();
	ButtonText1->SetText(L"Disable로 만들기\nHoverSound O");
	ButtonText1->SetFontSize(25.0f);
	ButtonText1->AttachToWidget(TestButton1);

	ButtonText2 = std::make_shared<FTextWidget>();
	ButtonText2->SetText(L"Normal로 만들기\nClickSound O");
	ButtonText2->SetFontSize(25.0f);
	ButtonText2->AttachToWidget(TestButton2);

	ButtonText3 = std::make_shared<FTextWidget>();
	ButtonText3->SetText(L"Image바꾸기");
	ButtonText3->SetFontSize(25.0f);
	ButtonText3->AttachToWidget(TestButton3);

	ButtonText1->SetHorizontalAlignment(ETextHorizontalAlignment::Left);
	ButtonText1->SetVerticalAlignment(ETextVerticalAlignment::Center);

	ButtonText2->SetHorizontalAlignment(ETextHorizontalAlignment::Center);
	ButtonText2->SetVerticalAlignment(ETextVerticalAlignment::Center);

	ButtonText3->SetHorizontalAlignment(ETextHorizontalAlignment::Right);
	ButtonText3->SetVerticalAlignment(ETextVerticalAlignment::Center);

	TestButton1->HoveredSound = USoundBase::GetSoundAsset("SB_CameraShutter");
	TestButton1->OnClicked.Add([this]()
	{
		this->SetButton3Active(false);
	});
	TestButton2->PressedSound = USoundBase::GetSoundAsset("SB_ButtonClick");
	TestButton2->OnClicked.Add([this]()
	{
		this->SetButton3Active(true);
	});
	TestButton3->HoveredSound = USoundBase::GetSoundAsset("SB_CameraShutter");
	TestButton3->PressedSound = USoundBase::GetSoundAsset("SB_ButtonClick");
	TestButton3->OnClicked.Add([this]()
	{
		this->ChangeImage();
	});
}
void UMyTestWidget::ChangeImage()
{
	static bool b = false;
	static std::array<std::shared_ptr<UTexture>, 3> Texture;
	if (!b)
	{
		b = true;
		Texture = {
			UTexture::GetTextureCache("T_CoolSword"),
			UTexture::GetTextureCache("T_Flame"),
			UTexture::GetTextureCache("T_BigFire")
		};
	}

	static int a= -1;
	++a;
	if (a >= 3)
	{
		a = 0;
	}
	TestImage1->SetBrush(FImageBrush{Texture[a]});
	
}
void UMyTestWidget::ChangeImage1()
{
	static bool bIsTrue = true;
	XMFLOAT4 Color = TestImage1->GetColor();
	Color.x -= 0.3f;
	Color.y -= 0.3f;
	Color.z -= 0.3f;
	if (Color.x <= 0.0f)
	{
		Color.x = 1.0f;
		Color.y = 1.0f;
		Color.z = 1.0f;
		const std::shared_ptr<UTexture>& Texture = bIsTrue ? UTexture::GetTextureCache("T_White") : UTexture::GetTextureCache("T_Cube");
		bIsTrue = !bIsTrue;
		TestImage1->SetBrush(FImageBrush{Texture});
	}
	TestImage1->SetColor(Color);
}
void UMyTestWidget::ChangeImage2()
{
	static bool bIsTrue = true;
	XMFLOAT4 Color = TestImage2->GetColor();
	Color.x -= 0.3f;
	Color.y -= 0.3f;
	Color.z -= 0.3f;
	if (Color.x <= 0.0f)
	{
		Color.x = 1.0f;
		Color.y = 1.0f;
		Color.z = 1.0f;
		const std::shared_ptr<UTexture>& Texture = bIsTrue ? UTexture::GetTextureCache("T_Cube") : UTexture::GetTextureCache("T_White");
		bIsTrue = !bIsTrue;
		TestImage2->SetBrush(FImageBrush{Texture});
	}
	TestImage2->SetColor(Color);
}

void UMyTestWidget::SetButton3Active(bool NewActive)
{
	TestButton3->SetDisabled(!NewActive);	
}


//TestImage1 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_Cube")}, XMFLOAT4{1,1,1,1});
//TestImage2 = std::make_shared<FImageWidget>(FImageBrush{UTexture::GetTextureCache("T_White")}, XMFLOAT4{1,1,1,1});

//TestImage1->AttachToWidget(TestVerticalBox1);
//TestImage2->AttachToWidget(TestVerticalBox1);

//if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TestImage1->GetSlot()))
//{
//	VerBoxSlot->FillSize = 1.0f;
//}
//TestImage1->OnMouseButtonDown.Add(this, &UMyTestWidget::ChangeImage1);

//if (const std::shared_ptr<FVerticalBoxSlot>& VerBoxSlot = std::dynamic_pointer_cast<FVerticalBoxSlot>(TestImage2->GetSlot()))
//{
//	VerBoxSlot->FillSize = 3.0f;
//}
//TestImage2->OnMouseButtonDown.Add(this, &UMyTestWidget::ChangeImage2);