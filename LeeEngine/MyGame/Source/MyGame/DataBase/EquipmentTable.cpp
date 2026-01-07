#include "CoreMinimal.h"
#include "EquipmentTable.h"

EquipmentTable::EquipmentTable(SQLiteWrapper& DB) : DB(DB) {}

bool EquipmentTable::CreateLevelTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS EquipmentLevels (
            UserID TEXT,
            EquipmentType TEXT,
            Level INTEGER DEFAULT 1,
            PRIMARY KEY (UserID, EquipmentType),
            FOREIGN KEY (UserID) REFERENCES Users(UserID)
        )
    )";

	return DB.Execute(Query);
}

bool EquipmentTable::CreateUpgradeTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS EquipmentUpgrade (
            EquipmentType TEXT PRIMARY KEY,
            BaseGold INTEGER,
            Multiplier REAL
        )
    )";

	return DB.Execute(Query);
}

bool EquipmentTable::SetEquipmentLevel(const std::string& UserID,
	EquipmentType Type,
	int Level)
{
	auto Stmt = DB.Prepare(R"(
        INSERT OR REPLACE INTO EquipmentLevels (UserID, EquipmentType, Level)
        VALUES (?, ?, ?)
    )");

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, EquipmentTypeToString(Type));
	Stmt->BindInt(3, Level);

	return Stmt->Step();
}

std::optional<int> EquipmentTable::GetEquipmentLevel(const std::string& UserID,
	EquipmentType Type)
{
	auto Stmt = DB.Prepare(R"(
        SELECT Level FROM EquipmentLevels
        WHERE UserID = ? AND EquipmentType = ?
    )");

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, EquipmentTypeToString(Type));

	if (Stmt->Step())
	{
		return Stmt->GetInt(0);
	}
	return std::nullopt;
}

bool EquipmentTable::SetUpgradeCost(EquipmentType Type, int BaseGold, double Multiplier)
{
	auto Stmt = DB.Prepare(R"(
        INSERT OR REPLACE INTO EquipmentUpgrade (EquipmentType, BaseGold, Multiplier)
        VALUES (?, ?, ?)
    )");

	Stmt->BindText(1, EquipmentTypeToString(Type));
	Stmt->BindInt(2, BaseGold);
	Stmt->BindDouble(3, Multiplier);

	return Stmt->Step();
}

std::optional<int> EquipmentTable::GetUpgradeCost(EquipmentType Type, int CurrentLevel)
{
	auto Stmt = DB.Prepare(R"(
        SELECT BaseGold, Multiplier FROM EquipmentUpgrade
        WHERE EquipmentType = ?
    )");

	Stmt->BindText(1, EquipmentTypeToString(Type));

	if (Stmt->Step())
	{
		int BaseGold = Stmt->GetInt(0);
		double Multiplier = Stmt->GetDouble(1);

		// 비용 계산: BaseGold * (Multiplier ^ CurrentLevel)
		int Cost = static_cast<int>(BaseGold * std::pow(Multiplier, CurrentLevel));
		return Cost;
	}
	return std::nullopt;
}

bool EquipmentTable::UpgradeEquipment(const std::string& UserID, EquipmentType Type)
{
	auto Stmt = DB.Prepare(R"(
        UPDATE EquipmentLevels
        SET Level = Level + 1
        WHERE UserID = ? AND EquipmentType = ?
    )");

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, EquipmentTypeToString(Type));

	return Stmt->Step();
}

std::string EquipmentTable::EquipmentTypeToString(EquipmentType Type)
{
	switch (Type)
	{
	case EquipmentType::Weapon: return "Weapon";
	case EquipmentType::Armor: return "Armor";
	case EquipmentType::Helmet: return "Helmet";
	case EquipmentType::Boots: return "Boots";
	default: return "Unknown";
	}
}

EquipmentType EquipmentTable::StringToEquipmentType(const std::string& Str)
{
	if (Str == "Weapon") return EquipmentType::Weapon;
	if (Str == "Armor") return EquipmentType::Armor;
	if (Str == "Helmet") return EquipmentType::Helmet;
	if (Str == "Boots") return EquipmentType::Boots;
	return EquipmentType::Weapon;
}

