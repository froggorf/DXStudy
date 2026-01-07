#include "CoreMinimal.h"
#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::Get()
{
	static DatabaseManager Instance;
	return Instance;
}

bool DatabaseManager::Initialize(const std::string& DBPath)
{
	if (!DB.Open(DBPath))
	{
		return false;
	}

	// 테이블 객체 생성
	UserTablePtr = std::make_unique<UserTable>(DB);
	EquipmentTablePtr = std::make_unique<EquipmentTable>(DB);
	GoldTablePtr = std::make_unique<GoldTable>(DB);
	StageTablePtr = std::make_unique<StageTable>(DB);

	// 테이블 생성
	UserTablePtr->CreateTable();
	EquipmentTablePtr->CreateLevelTable();
	EquipmentTablePtr->CreateUpgradeTable();
	GoldTablePtr->CreateTable();
	StageTablePtr->CreateTable();

	return true;
}

void DatabaseManager::Shutdown()
{
	UserTablePtr.reset();
	EquipmentTablePtr.reset();
	GoldTablePtr.reset();
	StageTablePtr.reset();

	DB.Close();
}

