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
	EEquipType Type,
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
	EEquipType Type)
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

bool EquipmentTable::SetUpgradeCost(EEquipType Type, int BaseGold, double Multiplier)
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

std::optional<int> EquipmentTable::GetUpgradeCost(EEquipType Type, int CurrentLevel)
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

bool EquipmentTable::UpgradeEquipment(const std::string& UserID, EEquipType Type)
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

std::string EquipmentTable::EquipmentTypeToString(EEquipType Type)
{
	switch (Type)
	{
	case EEquipType::Weapon: return "Weapon";
	case EEquipType::Armor: return "Armor";
	case EEquipType::Glove: return "Glove";
	case EEquipType::Head: return "Head";
	default: return "Unknown";
	}
}

EEquipType EquipmentTable::StringToEquipmentType(const std::string& Str)
{
	if (Str == "Weapon") return EEquipType::Weapon;
	if (Str == "Armor") return EEquipType::Armor;
	if (Str == "Glove") return EEquipType::Glove;
	if (Str == "Head") return EEquipType::Head;
	return EEquipType::Weapon;
}

