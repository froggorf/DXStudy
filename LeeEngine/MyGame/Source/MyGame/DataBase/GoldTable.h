#pragma once
#include "SQLiteWrapper.h"
#include <string>
#include <optional>

class GoldTable
{
public:
	GoldTable(SQLiteWrapper& DB);

	// 테이블 생성
	bool CreateTable();

	// 골드 설정
	bool SetGold(const std::string& UserID, int64_t Gold);

	// 골드 조회
	std::optional<int64_t> GetGold(const std::string& UserID);

	// 골드 추가
	bool AddGold(const std::string& UserID, int64_t Amount);

	// 골드 차감 (충분한지 확인 후)
	bool SpendGold(const std::string& UserID, int64_t Amount);

	// 골드 충분한지 확인
	bool HasEnoughGold(const std::string& UserID, int64_t Required);

private:
	SQLiteWrapper& DB;
};

