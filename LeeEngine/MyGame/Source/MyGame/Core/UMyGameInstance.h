#pragma once
#include "Engine/Core/UGameInstance.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class UMyGameInstance : public UGameInstance
{
	MY_GENERATE_BODY(UMyGameInstance)

	UMyGameInstance() = default;
	~UMyGameInstance() override = default;

	void LoadInitialData() override;
	void Login();

	const std::array<int, static_cast<int>(EEquipType::Count)>& GetEquipLevel() const {return EquipLevel;}
	UINT GetGold() const {return Gold;}

	void EnchantEquipLevel(EEquipType Type);

	std::array<int, static_cast<int>(EEquipType::Count)> EquipLevel;
	UINT Gold;


	std::string UserName = "";
};
