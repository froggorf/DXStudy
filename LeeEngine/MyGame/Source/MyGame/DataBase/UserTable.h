#pragma once
#include <string>
#include <optional>
#include "SQLiteWrapper.h"

class UserTable
{
public:
	UserTable(SQLiteWrapper& DB);

	bool CreateTable();
	bool RegisterUser(const std::string& UserID, const std::string& Password);
	bool ValidateLogin(const std::string& UserID, const std::string& Password);
	bool UserExists(const std::string& UserID);
	bool DeleteUser(const std::string& UserID);

	using OnUserCreatedCallback = std::function<bool(const std::string& UserID)>;
	void SetOnUserCreatedCallback(OnUserCreatedCallback Callback);

private:
	OnUserCreatedCallback OnUserCreated;

private:
	SQLiteWrapper& DB;

	std::string GenerateSalt(size_t Length = 16);
	std::string HashPassword(const std::string& Password, const std::string& Salt);
	std::optional<std::string> GetUserSalt(const std::string& UserID);

	// SHA256 직접 구현
	std::string SHA256(const std::string& Data);


};
