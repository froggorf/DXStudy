#include "CoreMinimal.h"
#include "StageTable.h"

StageTable::StageTable(SQLiteWrapper& DB) : DB(DB) {}

bool StageTable::CreateTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS StageLevel (
            UserID TEXT PRIMARY KEY,
            StageLevel INTEGER DEFAULT 1,
            UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (UserID) REFERENCES Users(UserID)
        )
    )";

	return DB.Execute(Query);
}

bool StageTable::SetStageLevel(const std::string& UserID, int64_t StageLevel)
{
	auto Stmt = DB.Prepare(R"(
        INSERT OR REPLACE INTO StageLevel (UserID, StageLevel, UpdatedAt)
        VALUES (?, ?, CURRENT_TIMESTAMP)
    )");

	Stmt->BindText(1, UserID);
	Stmt->BindInt64(2, StageLevel);

	return Stmt->Step();
}

std::optional<int64_t> StageTable::GetStageLevel(const std::string& UserID)
{
	auto Stmt = DB.Prepare(R"(
        SELECT StageLevel FROM StageLevel WHERE UserID = ?
    )");

	Stmt->BindText(1, UserID);

	if (Stmt->Step())
	{
		return Stmt->GetInt64(0);
	}
	return std::nullopt;
}

bool StageTable::IncreaseStageLevel(const std::string& UserID)
{
	auto Stmt = DB.Prepare(R"(
        UPDATE StageLevel 
        SET StageLevel = StageLevel + 1,
            UpdatedAt = CURRENT_TIMESTAMP
        WHERE UserID = ?
    )");

	Stmt->BindText(1, UserID);

	return Stmt->Step();
}

bool StageTable::SetStage(const std::string& UserID, int64_t StageLevel)
{
	return SetStageLevel(UserID, StageLevel);
}

bool StageTable::InitializeStage(const std::string& UserID)
{
	// 스테이지 1로 초기화
	return SetStageLevel(UserID, 1);
}

bool StageTable::CanProgressToNextStage(const std::string& UserID, int64_t RequiredStage)
{
	auto CurrentStage = GetStageLevel(UserID);

	if (!CurrentStage.has_value())
	{
		return false;
	}

	return CurrentStage.value() >= RequiredStage;
}
