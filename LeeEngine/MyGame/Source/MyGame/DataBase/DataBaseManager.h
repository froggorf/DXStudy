// DatabaseManager.h
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "SQLiteWrapper.h"
#include "UserTable.h"
#include "EquipmentTable.h"
#include "GoldTable.h"
#include "StageTable.h"

class DatabaseManager
{
public:
	static DatabaseManager& Get();

	bool Initialize(const std::string& DBPath);
	void Shutdown();

	// 테이블 접근자
	UserTable* GetUserTable() { return UserTablePtr.get(); }
	EquipmentTable* GetEquipmentTable() { return EquipmentTablePtr.get(); }
	GoldTable* GetGoldTable() { return GoldTablePtr.get(); }
	StageTable* GetStageTable() { return StageTablePtr.get(); }

	bool TryUpgradeEquipment(const std::string& UserID, EEquipType Type, int& OutNewLevel, int64_t& OutNewGold, int& OutCost);

private:
	DatabaseManager() = default;
	~DatabaseManager() = default;
	DatabaseManager(const DatabaseManager&) = delete;
	DatabaseManager& operator=(const DatabaseManager&) = delete;

	// 새 유저 초기 데이터 생성
	bool InitializeNewUser(const std::string& UserID);

	SQLiteWrapper DB;
	std::unique_ptr<UserTable> UserTablePtr;
	std::unique_ptr<EquipmentTable> EquipmentTablePtr;
	std::unique_ptr<GoldTable> GoldTablePtr;
	std::unique_ptr<StageTable> StageTablePtr;
};
