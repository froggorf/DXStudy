#pragma once
#include "Engine/Core/UGameInstance.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class UMyGameInstance : public UGameInstance
{
	MY_GENERATE_BODY(UMyGameInstance)

	UMyGameInstance() = default;
	~UMyGameInstance() override = default;

	void LoadInitialData() override;
	bool TryLogin(const std::string& ID, const std::string& PW);

	const std::array<int, static_cast<int>(EEquipType::Count)>& GetEquipLevel() const {return EquipLevel;}
	UINT GetGold() const {return Gold;}

	void EnchantEquipLevel(EEquipType Type);

	std::array<int, static_cast<int>(EEquipType::Count)> EquipLevel = {0,0,0,0};
	UINT Gold = 0;
	UINT StageLevel = 1;


	std::string UserName = "";
};
