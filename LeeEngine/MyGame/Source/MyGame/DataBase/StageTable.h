#pragma once
#include "SQLiteWrapper.h"
#include <string>
#include <optional>

class StageTable
{
public:
	StageTable(SQLiteWrapper& DB);

	// 테이블 생성
	bool CreateTable();

	// 스테이지 레벨 설정
	bool SetStageLevel(const std::string& UserID, int64_t StageLevel);

	// 스테이지 레벨 조회
	std::optional<int64_t> GetStageLevel(const std::string& UserID);

	// 스테이지 레벨 증가
	bool IncreaseStageLevel(const std::string& UserID);

	// 특정 스테이지로 설정 (치트/디버그용)
	bool SetStage(const std::string& UserID, int64_t StageLevel);

	// 스테이지 초기화 (유저 생성 시)
	bool InitializeStage(const std::string& UserID);

	// 다음 스테이지로 진행 가능한지 확인
	bool CanProgressToNextStage(const std::string& UserID, int64_t RequiredStage);

private:
	SQLiteWrapper& DB;
};
