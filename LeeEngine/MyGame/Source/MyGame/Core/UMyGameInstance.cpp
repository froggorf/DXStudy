#include "CoreMinimal.h"
#include "UMyGameInstance.h"

#include "MyGame/DataBase/DatabaseManager.h"

namespace
{
	std::string ResolveDatabasePath()
	{
		namespace fs = std::filesystem;

		fs::path base = fs::current_path();
		for (int i = 0; i < 6; ++i)
		{
			fs::path projectDb = base / "MyGame" / "MyGame.db";
			if (fs::exists(projectDb))
			{
				return projectDb.generic_string();
			}

			fs::path directDb = base / "MyGame.db";
			if (fs::exists(directDb))
			{
				return directDb.generic_string();
			}

			if (!base.has_parent_path())
			{
				break;
			}
			base = base.parent_path();
		}

		return (fs::current_path() / "MyGame.db").generic_string();
	}
}

void UMyGameInstance::LoadInitialData()
{
	if (UserName.empty())
	{
		assert(false);
	}

	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return;
	}

	if (auto UserGold = DBMgr.GetGoldTable()->GetGold(UserName))
	{
		Gold = *UserGold;
	}

	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		if (auto UserEquipData = DBMgr.GetEquipmentTable()->GetEquipmentLevel(UserName, static_cast<EEquipType>(i)))
		{
			EquipLevel[i] = *UserEquipData;
		}
	}

	if (auto StageData = DBMgr.GetStageTable()->GetStageLevel(UserName))
	{
		StageLevel = *StageData;
	}
}



bool UMyGameInstance::TryLogin(const std::string& ID, const std::string& PW)
{
	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return false;
	}

	return DBMgr.GetUserTable()->ValidateLogin(ID, PW);
}

bool UMyGameInstance::TryRegister(const std::string& ID, const std::string& PW)
{
	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return false;
	}

	if (DBMgr.GetUserTable()->UserExists(ID))
	{
		return false;
	}

	return DBMgr.GetUserTable()->RegisterUser(ID, PW);
}

bool UMyGameInstance::GetEquipUpgradeCost(EEquipType Type, int& OutCost) const
{
	OutCost = 0;
	if (UserName.empty())
	{
		return false;
	}

	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return false;
	}

	EquipmentTable* EquipTable = DBMgr.GetEquipmentTable();
	if (!EquipTable)
	{
		return false;
	}

	const int CurrentLevel = EquipLevel[static_cast<int>(Type)];
	if (const std::optional<int> Cost = EquipTable->GetUpgradeCost(Type, CurrentLevel))
	{
		OutCost = max(0, Cost.value());
		return true;
	}

	return false;
}

bool UMyGameInstance::AddGold(int64_t Amount)
{
	if (UserName.empty())
	{
		return false;
	}

	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return false;
	}

	GoldTable* GoldTablePtr = DBMgr.GetGoldTable();
	if (!GoldTablePtr)
	{
		return false;
	}

	if (!GoldTablePtr->AddGold(UserName, Amount))
	{
		return false;
	}

	if (const std::optional<int64_t> CurrentGold = GoldTablePtr->GetGold(UserName))
	{
		Gold = static_cast<UINT>(std::max<int64_t>(0, CurrentGold.value()));
	}

	return true;
}

EEquipUpgradeResult UMyGameInstance::EnchantEquipLevel(EEquipType Type)
{
	if (UserName.empty())
	{
		return EEquipUpgradeResult::InvalidUser;
	}

	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize(ResolveDatabasePath()))
	{
		return EEquipUpgradeResult::DbError;
	}

	EquipmentTable* EquipTable = DBMgr.GetEquipmentTable();
	GoldTable* GoldTablePtr = DBMgr.GetGoldTable();
	if (!EquipTable || !GoldTablePtr)
	{
		return EEquipUpgradeResult::DbError;
	}

	const std::optional<int> CurrentLevel = EquipTable->GetEquipmentLevel(UserName, Type);
	const std::optional<int64_t> CurrentGold = GoldTablePtr->GetGold(UserName);
	if (!CurrentLevel.has_value() || !CurrentGold.has_value())
	{
		return EEquipUpgradeResult::NoData;
	}

	const std::optional<int> UpgradeCost = EquipTable->GetUpgradeCost(Type, CurrentLevel.value());
	if (!UpgradeCost.has_value())
	{
		return EEquipUpgradeResult::NoData;
	}

	if (CurrentGold.value() < UpgradeCost.value())
	{
		return EEquipUpgradeResult::NotEnoughGold;
	}

	int NewLevel = 0;
	int64_t NewGold = 0;
	int OutUpgradeCost = 0;
	if (!DBMgr.TryUpgradeEquipment(UserName, Type, NewLevel, NewGold, OutUpgradeCost))
	{
		return EEquipUpgradeResult::DbError;
	}

	EquipLevel[static_cast<int>(Type)] = NewLevel;
	Gold = static_cast<UINT>(std::max<int64_t>(0, NewGold));

	return EEquipUpgradeResult::Success;
}
