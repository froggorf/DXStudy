#pragma once
#include "sqlite3.h"
#include <string>
#include <functional>
#include <memory>

class SQLiteWrapper
{
public:
	SQLiteWrapper() = default;
	~SQLiteWrapper();

	// DB 연결
	bool Open(const std::string& DBPath);
	void Close();
	bool IsOpen() const{ return DB != nullptr; }

	// 쿼리 실행
	bool Execute(const std::string& Query);

	// SELECT 쿼리 (콜백 방식)
	using RowCallback = std::function<void(sqlite3_stmt*)>;
	bool ExecuteQuery(const std::string& Query, RowCallback Callback);

	// Prepared Statement
	class Statement
	{
	public:
		Statement(sqlite3* DB, const std::string& Query);
		~Statement();

		// 바인딩
		void BindInt(int Index, int Value);
		void BindInt64(int Index, int64_t Value);
		void BindDouble(int Index, double Value);
		void BindText(int Index, const std::string& Value);

		// 실행
		bool Step();
		void Reset();

		// 결과 가져오기
		int GetInt(int Column);
		int64_t GetInt64(int Column);
		double GetDouble(int Column);
		std::string GetText(int Column);

	private:
		sqlite3_stmt* Stmt = nullptr;
	};

	std::unique_ptr<Statement> Prepare(const std::string& Query);

	// 트랜잭션
	bool BeginTransaction();
	bool Commit();
	bool Rollback();

	// 에러 처리
	std::string GetLastError() const;

private:
	sqlite3* DB = nullptr;
};

