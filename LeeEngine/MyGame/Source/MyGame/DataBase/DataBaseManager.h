#pragma once
#include "SQLiteWrapper.h"
#include "UserTable.h"
#include "EquipmentTable.h"
#include "GoldTable.h"
#include "StageTable.h"
#include <memory>
#include <string>

class DatabaseManager
{
public:
	static DatabaseManager& Get();

	// 초기화
	bool Initialize(const std::string& DBPath);
	void Shutdown();

	// 테이블 접근자
	UserTable& Users(){ return *UserTablePtr; }
	EquipmentTable& Equipment(){ return *EquipmentTablePtr; }
	GoldTable& Gold(){ return *GoldTablePtr; }
	StageTable& Stage(){ return *StageTablePtr; }

	// 트랜잭션
	bool BeginTransaction(){ return DB.BeginTransaction(); }
	bool Commit(){ return DB.Commit(); }
	bool Rollback(){ return DB.Rollback(); }

private:
	DatabaseManager() = default;
	~DatabaseManager() = default;

	DatabaseManager(const DatabaseManager&) = delete;
	DatabaseManager& operator=(const DatabaseManager&) = delete;

	SQLiteWrapper DB;
	std::unique_ptr<UserTable> UserTablePtr;
	std::unique_ptr<EquipmentTable> EquipmentTablePtr;
	std::unique_ptr<GoldTable> GoldTablePtr;
	std::unique_ptr<StageTable> StageTablePtr;
};

