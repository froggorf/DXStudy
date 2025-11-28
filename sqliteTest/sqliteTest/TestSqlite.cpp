#include <iostream>
#include <windows.h>

#include "sqlite3.h"



namespace LeeSQLite
{
	struct SQLiteDeleter
	{
		void operator()(sqlite3* DB)
		{
			if (DB)
			{
				std::cout<< "커스텀 딜리터 작동중\n";
				sqlite3_close(DB);
			}
		}
	};

	std::shared_ptr<sqlite3> Make_SQLiteShared(const std::string& DBFileName)
	{
		sqlite3* RawDB = nullptr;
		int Result = sqlite3_open(DBFileName.c_str(), &RawDB);
		if (Result != SQLITE_OK)
		{
			if (RawDB)
			{
				sqlite3_close(RawDB);
			}
			return nullptr;
		}

		// NOTE: make_shared는 내부에서 만드는것이기에 만들고 건네줄 수 없음
		// 또한 커스텀 딜리터도 지원 안한다고함
		// make_shared를 사용할 수 없는 상황에서 캐시미스가 날 수 있지만,
		// 성능차이는 느껴질정도는 아니라고함
		//std::make_shared<sqlite3>(RawDB, SQLiteDeleter());

		// NOTE2 : 현대 C++에선 다음과 같이 반환문을 간결히 작성하라고 함
		//return std::shared_ptr<sqlite3>(RawDB, SQLiteDeleter());
		return {RawDB, SQLiteDeleter()};
	}

	class LoginDB
	{
		//static bool RegisterUser()
	};
}


int main()
{
	std::shared_ptr<sqlite3> DB = LeeSQLite::Make_SQLiteShared("Test.db");	
	
	if (!DB)
	{
		std::cout<<"데이터 베이스 열기 실패: "<<sqlite3_errmsg(DB.get()) << '\n';
		return 1;
	}

	std::cout<<"데이터베이스 연결 완료"<<"\n";


	return 0;
}