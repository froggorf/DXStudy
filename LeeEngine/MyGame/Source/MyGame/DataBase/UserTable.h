#pragma once
#include "SQLiteWrapper.h"
#include <string>
#include <optional>

class UserTable
{
public:
	UserTable(SQLiteWrapper& DB);

	// 테이블 생성
	bool CreateTable();

	// 유저 등록
	bool RegisterUser(const std::string& UserID,
		const std::string& Email,
		const std::string& PasswordHash);

	// 로그인 검증
	bool ValidateLogin(const std::string& UserID,
		const std::string& PasswordHash);

	// 유저 존재 확인
	bool UserExists(const std::string& UserID);

	// 이메일로 유저 찾기
	std::optional<std::string> GetUserIDByEmail(const std::string& Email);

	// 유저 삭제
	bool DeleteUser(const std::string& UserID);

private:
	SQLiteWrapper& DB;
};

