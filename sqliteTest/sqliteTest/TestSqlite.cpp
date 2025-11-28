#include <iostream>
#include <windows.h>

#include "sqlite3.h"


struct FUserInfo
{
	int UserID;
	std::string UserName;
	std::string Email;
	std::string Password;
	std::string CreateAt;
	std::string LastLogin;
};

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
}



class FLoginDB
{
public:

	static bool RegisterUser(const std::string& Email, const std::string& PW)
	{
		
	}

	static bool TryLogin(const std::shared_ptr<sqlite3>& LoginDB, const std::string& ID, const std::string& PW)
	{
		sqlite3_stmt* STMT = nullptr;
		const char* SQL = "SELECT user_id FROM users WHERE email = ? AND password = ?;";
		if (sqlite3_prepare_v2(LoginDB.get(), SQL, -1, &STMT, nullptr) != SQLITE_OK)
		{
			std::cout<<"SQL 준비 오류: "<<sqlite3_errmsg(LoginDB.get()) <<"\n";
			return false;
		}

		sqlite3_bind_text(STMT, 1, ID.c_str(), -1 ,SQLITE_STATIC);
		sqlite3_bind_text(STMT, 2, PW.c_str(), -1 ,SQLITE_STATIC);

		bool LoginSuccess = (sqlite3_step(STMT) == SQLITE_ROW);
		sqlite3_finalize(STMT);

		if (LoginSuccess)
		{
			std::cout<<"로그인 성공\n";
			return true;
		}
		else
		{
			std::cout<<"이메일 또는 비밀번호가 일치하지 않습니다.\n";
			return false;
		}
	}
};


int main()
{
	std::shared_ptr<sqlite3> DB = LeeSQLite::Make_SQLiteShared("Test.db");	
	
	if (!DB)
	{
		std::cout<<"데이터 베이스 열기 실패: "<<sqlite3_errmsg(DB.get()) << '\n';
		return 1;
	}

	std::cout<<"데이터베이스 연결 완료"<<"\n";


	while (true)
	{
		std::cout<<"\n\n===========================================\n";
		std::cout<<"0 : 로그인\n";
		std::cout<<"1 : 회원가입\n";
		std::cout<<"2 : 종료\n";

		int Command;
		std::cout<<"명령을 입력하세요 : ";
		std::cin>>Command;

		switch (Command)
		{
		case 0:
			{
				std::string ID;
				std::string PW;
				std::cout << "ID : ";
				std::cin>>ID;
				std::cout<<"Password : ";
				std::cin>>PW;
				FLoginDB::TryLogin(DB, ID, PW);
			}
		break;
		case 1:
		break;
		case 2:
			std::cout<<"로그인 시스템을 종료합니다.\n";
			return 0;	
		break;
		}
	}

	return 0;
}