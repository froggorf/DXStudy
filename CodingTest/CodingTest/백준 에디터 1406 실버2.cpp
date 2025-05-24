#include <iostream>
#include <list>
// 배열/벡터 -> 임의접근은 가능하지만 추가하는데에 O(N)
// 덱 -> 임의삽입이 안됨
// 리스트..

int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	std::list<char> Text;
	std::string InitText;
	std::cin>>InitText;
	for(size_t i = 0; i < InitText.length(); ++i)
	{
		Text.emplace_back(InitText[i]);
	}

	auto Cursor = Text.end();

	int N;
	std::cin>>N;
	
	for(int i = 0; i < N; ++i)
	{
		char Command;
		std::cin>>Command;
		switch(Command)
		{
		case 'L':
		if(Cursor!=Text.begin())
		{
			--Cursor;
		}
		break;
		case 'D':
		if(Cursor!=Text.end())
		{
			++Cursor;
		}
		break;
		case 'B':
		if(Cursor!=Text.begin())
		{
			--Cursor;
			Cursor = Text.erase(Cursor);
		}
		break;
		case 'P':
		{
				char NewT;
				std::cin>>NewT;
			Text.emplace(Cursor,NewT);
		}
		
		break;

		default:
		break;
		}
	}

	for(auto p = Text.begin(); p!=Text.end(); ++p)
	{
		std::cout<<*p;
	}
}