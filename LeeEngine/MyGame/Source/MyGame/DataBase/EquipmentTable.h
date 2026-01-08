#pragma once
#include "SQLiteWrapper.h"
#include <string>
#include <optional>
#include <vector>

#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class EquipmentTable
{
public:
	EquipmentTable(SQLiteWrapper& DB);

	// 테이블 생성
	bool CreateLevelTable();
	bool CreateUpgradeTable();

	// 장비 레벨 관리
	bool SetEquipmentLevel(const std::string& UserID,
		EEquipType Type,
		int Level);

	std::optional<int> GetEquipmentLevel(const std::string& UserID,
		EEquipType Type);

	// 장비 강화 비용
	bool SetUpgradeCost(EEquipType Type, int BaseGold, double Multiplier);

	std::optional<int> GetUpgradeCost(EEquipType Type, int CurrentLevel);

	// 장비 강화
	bool UpgradeEquipment(const std::string& UserID, EEquipType Type);

private:
	SQLiteWrapper& DB;

	std::string EquipmentTypeToString(EEquipType Type);
	EEquipType StringToEquipmentType(const std::string& Str);
};

