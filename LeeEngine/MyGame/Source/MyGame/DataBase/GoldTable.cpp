#include "CoreMinimal.h"
#include "GoldTable.h"

GoldTable::GoldTable(SQLiteWrapper& DB) : DB(DB) {}

bool GoldTable::CreateTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS Gold (
            UserID TEXT PRIMARY KEY,
            Amount INTEGER DEFAULT 0,
            FOREIGN KEY (UserID) REFERENCES Users(UserID)
        )
    )";

	return DB.Execute(Query);
}

bool GoldTable::SetGold(const std::string& UserID, int64_t Gold)
{
	auto Stmt = DB.Prepare(R"(
        INSERT OR REPLACE INTO Gold (UserID, Amount)
        VALUES (?, ?)
    )");

	Stmt->BindText(1, UserID);
	Stmt->BindInt64(2, Gold);

	return Stmt->Step();
}

std::optional<int64_t> GoldTable::GetGold(const std::string& UserID)
{
	auto Stmt = DB.Prepare("SELECT Amount FROM Gold WHERE UserID = ?");
	Stmt->BindText(1, UserID);

	if (Stmt->Step())
	{
		return Stmt->GetInt64(0);
	}
	return std::nullopt;
}

bool GoldTable::AddGold(const std::string& UserID, int64_t Amount)
{
	auto Stmt = DB.Prepare(R"(
        UPDATE Gold SET Amount = Amount + ? WHERE UserID = ?
    )");

	Stmt->BindInt64(1, Amount);
	Stmt->BindText(2, UserID);

	return Stmt->Step();
}

bool GoldTable::SpendGold(const std::string& UserID, int64_t Amount)
{
	// 트랜잭션 시작
	if (!DB.BeginTransaction())
		return false;

	// 골드 확인
	if (!HasEnoughGold(UserID, Amount))
	{
		DB.Rollback();
		return false;
	}

	// 골드 차감
	auto Stmt = DB.Prepare(R"(
        UPDATE Gold SET Amount = Amount - ? WHERE UserID = ?
    )");

	Stmt->BindInt64(1, Amount);
	Stmt->BindText(2, UserID);

	if (!Stmt->Step())
	{
		DB.Rollback();
		return false;
	}

	return DB.Commit();
}

bool GoldTable::HasEnoughGold(const std::string& UserID, int64_t Required)
{
	auto CurrentGold = GetGold(UserID);
	return CurrentGold.has_value() && CurrentGold.value() >= Required;
}

