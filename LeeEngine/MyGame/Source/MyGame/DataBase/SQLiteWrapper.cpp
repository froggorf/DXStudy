#include "CoreMinimal.h"
#include "SQLiteWrapper.h"

SQLiteWrapper::~SQLiteWrapper()
{
	Close();
}

bool SQLiteWrapper::Open(const std::string& DBPath)
{
	int Result = sqlite3_open(DBPath.c_str(), &DB);
	if (Result != SQLITE_OK)
	{
		std::cerr << "Failed to open database: " << sqlite3_errmsg(DB) << std::endl;
		DB = nullptr;
		return false;
	}
	return true;
}

void SQLiteWrapper::Close()
{
	if (DB)
	{
		sqlite3_close(DB);
		DB = nullptr;
	}
}

bool SQLiteWrapper::Execute(const std::string& Query)
{
	char* ErrMsg = nullptr;
	int Result = sqlite3_exec(DB, Query.c_str(), nullptr, nullptr, &ErrMsg);

	if (Result != SQLITE_OK)
	{
		std::cerr << "SQL error: " << ErrMsg << std::endl;
		sqlite3_free(ErrMsg);
		return false;
	}
	return true;
}

bool SQLiteWrapper::ExecuteQuery(const std::string& Query, RowCallback Callback)
{
	sqlite3_stmt* Stmt;
	int Result = sqlite3_prepare_v2(DB, Query.c_str(), -1, &Stmt, nullptr);

	if (Result != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << std::endl;
		return false;
	}

	while (sqlite3_step(Stmt) == SQLITE_ROW)
	{
		Callback(Stmt);
	}

	sqlite3_finalize(Stmt);
	return true;
}

std::unique_ptr<SQLiteWrapper::Statement> SQLiteWrapper::Prepare(const std::string& Query)
{
	return std::make_unique<Statement>(DB, Query);
}

bool SQLiteWrapper::BeginTransaction()
{
	return Execute("BEGIN TRANSACTION");
}

bool SQLiteWrapper::Commit()
{
	return Execute("COMMIT");
}

bool SQLiteWrapper::Rollback()
{
	return Execute("ROLLBACK");
}

std::string SQLiteWrapper::GetLastError() const
{
	return DB ? sqlite3_errmsg(DB) : "Database not open";
}

// Statement 구현
SQLiteWrapper::Statement::Statement(sqlite3* DB, const std::string& Query)
{
	sqlite3_prepare_v2(DB, Query.c_str(), -1, &Stmt, nullptr);
}

SQLiteWrapper::Statement::~Statement()
{
	if (Stmt)
	{
		sqlite3_finalize(Stmt);
	}
}

void SQLiteWrapper::Statement::BindInt(int Index, int Value)
{
	sqlite3_bind_int(Stmt, Index, Value);
}

void SQLiteWrapper::Statement::BindInt64(int Index, int64_t Value)
{
	sqlite3_bind_int64(Stmt, Index, Value);
}

void SQLiteWrapper::Statement::BindDouble(int Index, double Value)
{
	sqlite3_bind_double(Stmt, Index, Value);
}

void SQLiteWrapper::Statement::BindText(int Index, const std::string& Value)
{
	sqlite3_bind_text(Stmt, Index, Value.c_str(), -1, SQLITE_TRANSIENT);
}

bool SQLiteWrapper::Statement::Step()
{
	return sqlite3_step(Stmt) == SQLITE_ROW;
}

void SQLiteWrapper::Statement::Reset()
{
	sqlite3_reset(Stmt);
}

int SQLiteWrapper::Statement::GetInt(int Column)
{
	return sqlite3_column_int(Stmt, Column);
}

int64_t SQLiteWrapper::Statement::GetInt64(int Column)
{
	return sqlite3_column_int64(Stmt, Column);
}

double SQLiteWrapper::Statement::GetDouble(int Column)
{
	return sqlite3_column_double(Stmt, Column);
}

std::string SQLiteWrapper::Statement::GetText(int Column)
{
	const char* Text = reinterpret_cast<const char*>(sqlite3_column_text(Stmt, Column));
	return Text ? Text : "";
}

