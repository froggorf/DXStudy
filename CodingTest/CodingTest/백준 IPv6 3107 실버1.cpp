#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
	// 앞의 0은 생략 가능
	// :0:00: 이런건 :: 으로 바꿀수있음
	// 한번만 사용가능하대 근데

	// 축약형 IPv6 가 주어질떄 역으로 만들어봐라
	std::string S;
	std::cin>>S;
	
	bool bIsFinal = false;
	std::string Answer;
	int Index = 0;
	while (Index < static_cast<int>(S.length()))
	{
		// 0000:0000 이 축약된 것이므로 처리해야함
		if (S[Index] == ':')
		{
			// 문장의 좌측 콜론개수, 우측 콜론개수를 세서 그만큼 채우면 될 것 같음
			int BeforeColonCount = 0;
			int AfterColonCount = 0;
			for (int i = 0; i < Index; ++i)
			{
				if (S[i] == ':')
				{
					++BeforeColonCount;
				}
			}
			for (int i = Index+ 2; i < S.length(); ++i)
			{
				if (S[i] == ':')
				{
					++AfterColonCount;
				}
			}
			
			std::string Cur;
			// 원래 7개가 존재해야하는데
			// 좌측 N개 우측 M 개, 7-N-M개의 0000이 연속으로 존재하는것
			int Count = 7-BeforeColonCount-AfterColonCount-1;
			if (Index == 0)
			{
				Cur = "0000:";
			}
			for (int i = 0; i < Count; ++i)
			{
				Cur = Cur+"0000:";	
			}
			if (Index+2 >= S.length())
			{
				Cur = Cur+"0000:";
			}
			Answer = Answer + Cur;
			if (Answer.length() > 38)
			{
				Answer.pop_back();
			}
			Index = Index+2;
			
			// ::1 -> 0 0 / 7개
			// 1::1 -> 
			// 1:: -> 0 0 / 7개
			// xxxx:xxx:xxxx::xxxx:xxx:xxxx

			continue;
		}


		bool bIsFinal = false;
		int NextColonIndex = Index;
		while (true)
		{
			++NextColonIndex;
			if (S[NextColonIndex] == ':' || NextColonIndex == S.length())
			{
				bIsFinal = NextColonIndex == S.length();
				break;
			}
		}
		std::string Cur;

		
		
		Cur.reserve(5);
		for (int i = Index; i < NextColonIndex; ++i)
		{
			Cur += S[i];
		}
		for (int i = Cur.length(); i < 4; ++i)
		{
			Cur = "0" + Cur;
		}

		if (!bIsFinal)
		{
			Cur = Cur+":";
		}
		Answer.append(Cur);

		if (NextColonIndex+1 < S.length() && S[NextColonIndex+1] == ':')
		{
			Index = NextColonIndex;
		}
		else
		{
			Index = NextColonIndex + 1;
		}
	}

	std::cout<<Answer;
}