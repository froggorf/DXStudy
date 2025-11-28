#include <iostream>
#include <windows.h>

#include "sqlite3.h"

int main()
{
	sqlite3* db;
	int rc = sqlite3_open("Test.db", &db);

	if (rc)
	{
		std::cout<<"데이터 베이스 열기 실패: "<<sqlite3_errmsg(db) << '\n';
		return 1;
	}

	std::cout<<"데이터베이스 연결 완료"<<"\n";

	std::string createTableSQL =
		"CREATE TABLE IF NOT EXISTS users("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL,"
		"email TEXT UNIQUE,"
		"age INTEGER"
		");";

	char* errMsg = nullptr;
	rc = sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK)
	{
		std::cout <<"SQL 오류: "<< (errMsg? errMsg : "알수없는 오류") << '\n';
		if (errMsg) sqlite3_free(errMsg);
	}
	else
	{
		std::cout << "테이블 생성 완료\n";
	}

	sqlite3_close(db);
	return 0;

}