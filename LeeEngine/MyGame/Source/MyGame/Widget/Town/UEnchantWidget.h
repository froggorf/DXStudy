#pragma once
#include "Engine/UEngine.h"
#include "Engine/Widget/UUserWidget.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class UEnchantWidget : public UUserWidget
{
	MY_GENERATE_BODY(UEnchantWidget)
	UEnchantWidget() = default;
	~UEnchantWidget() override = default;

	void NativeConstruct() override;

	void Enchant(EEquipType Type);
public:
	void Tick(float DeltaSeconds) override;
	void Close();
protected:
	std::shared_ptr<FButtonWidget> CloseButton;

	std::shared_ptr<FImageWidget> BaseImageWidget;
		std::shared_ptr<FHorizontalBoxWidget> HorBox;
			std::shared_ptr<FVerticalBoxWidget> VerBox[static_cast<int>(EEquipType::Count)];
				std::shared_ptr<FImageWidget> EquipImage[static_cast<int>(EEquipType::Count)];
				std::shared_ptr<FImageWidget> Dummy[static_cast<int>(EEquipType::Count)];
				std::shared_ptr<FButtonWidget> EquipUpButton[static_cast<int>(EEquipType::Count)];
					std::shared_ptr<FTextWidget> EquipText[static_cast<int>(EEquipType::Count)];
};
