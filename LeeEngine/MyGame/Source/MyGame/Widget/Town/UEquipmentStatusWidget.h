#pragma once

#include "Engine/Widget/UUserWidget.h" // Includes ChildWidget.h where F...Widgets are defined
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class UEquipmentStatusWidget : public UUserWidget
{
	MY_GENERATE_BODY(UEquipmentStatusWidget)

public:
	UEquipmentStatusWidget() = default;
	~UEquipmentStatusWidget() override = default;

	void NativeConstruct() override;

private:
	void UpdateEquipmentData();

	std::shared_ptr<class FImageWidget> BackgroundImage; 
	std::shared_ptr<class FHorizontalBoxWidget> EquipmentSlotsBox;
	std::shared_ptr<class FVerticalBoxWidget> SlotVerticalBox[static_cast<int>(EEquipType::Count)];
	std::shared_ptr<class FImageWidget> EquipImage[static_cast<int>(EEquipType::Count)];
	std::shared_ptr<class FImageWidget> TextBackground[static_cast<int>(EEquipType::Count)];
	std::shared_ptr<class FTextWidget> EquipLevelText[static_cast<int>(EEquipType::Count)];
};
