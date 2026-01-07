#pragma once
#include "SQLiteWrapper.h"
#include <string>
#include <optional>
#include <vector>

enum class EquipmentType
{
	Weapon,
	Armor,
	Helmet,
	Boots
};

class EquipmentTable
{
public:
	EquipmentTable(SQLiteWrapper& DB);

	// 테이블 생성
	bool CreateLevelTable();
	bool CreateUpgradeTable();

	// 장비 레벨 관리
	bool SetEquipmentLevel(const std::string& UserID,
		EquipmentType Type,
		int Level);

	std::optional<int> GetEquipmentLevel(const std::string& UserID,
		EquipmentType Type);

	// 장비 강화 비용
	bool SetUpgradeCost(EquipmentType Type, int BaseGold, double Multiplier);

	std::optional<int> GetUpgradeCost(EquipmentType Type, int CurrentLevel);

	// 장비 강화
	bool UpgradeEquipment(const std::string& UserID, EquipmentType Type);

private:
	SQLiteWrapper& DB;

	std::string EquipmentTypeToString(EquipmentType Type);
	EquipmentType StringToEquipmentType(const std::string& Str);
};

