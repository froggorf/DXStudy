#include "CoreMinimal.h"
#include "UserTable.h"

UserTable::UserTable(SQLiteWrapper& DB) : DB(DB) {}

bool UserTable::CreateTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS Users (
            UserID TEXT PRIMARY KEY,
            Email TEXT UNIQUE NOT NULL,
            PasswordHash TEXT NOT NULL,
            CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

	return DB.Execute(Query);
}

bool UserTable::RegisterUser(const std::string& UserID,
	const std::string& Email,
	const std::string& PasswordHash)
{
	auto Stmt = DB.Prepare(
		"INSERT INTO Users (UserID, Email, PasswordHash) VALUES (?, ?, ?)"
	);

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, Email);
	Stmt->BindText(3, PasswordHash);

	return Stmt->Step();
}

bool UserTable::ValidateLogin(const std::string& UserID,
	const std::string& PasswordHash)
{
	auto Stmt = DB.Prepare(
		"SELECT COUNT(*) FROM Users WHERE UserID = ? AND PasswordHash = ?"
	);

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, PasswordHash);

	if (Stmt->Step())
	{
		return Stmt->GetInt(0) > 0;
	}
	return false;
}

bool UserTable::UserExists(const std::string& UserID)
{
	auto Stmt = DB.Prepare(
		"SELECT COUNT(*) FROM Users WHERE UserID = ?"
	);

	Stmt->BindText(1, UserID);

	if (Stmt->Step())
	{
		return Stmt->GetInt(0) > 0;
	}
	return false;
}

std::optional<std::string> UserTable::GetUserIDByEmail(const std::string& Email)
{
	auto Stmt = DB.Prepare(
		"SELECT UserID FROM Users WHERE Email = ?"
	);

	Stmt->BindText(1, Email);

	if (Stmt->Step())
	{
		return Stmt->GetText(0);
	}
	return std::nullopt;
}

bool UserTable::DeleteUser(const std::string& UserID)
{
	auto Stmt = DB.Prepare("DELETE FROM Users WHERE UserID = ?");
	Stmt->BindText(1, UserID);
	return Stmt->Step();
}

