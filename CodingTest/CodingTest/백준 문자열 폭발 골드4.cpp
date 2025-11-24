#include <iostream>
#include <vector>
#include <stack>
#include <deque>
int main()
{
	std::string S;
	std::cin>>S;

	std::string BombStr;
	std::cin>>BombStr;

	// 최악의 경우엔 CCCCCCCCCCCCCC4444444444444 같은 문자열로 100만개가 있다면
	// 50만번 * 50만번 = 250'000'000'000 번 작동하므로 절대 시뮬레이션은 안됨
	// 그렇다면...

	// 오 스택을 사용할 수 있지않을까?
	// 근데 N개에 대해서 확인을 해야하니까
	// Deque를 쓰는 법도 있을것같음
	// 근데 그러면 vector 랑 다를게없네?
	// 벡터로 하고나서 스택처럼 활용해보면 될것같음.

	std::vector<char> Stack;
	char FinalBombChar = BombStr[BombStr.size()-1];
	size_t BombStrLength = BombStr.length();
	for (const char Character : S)
	{
		Stack.emplace_back(Character);
		// 마지막 문자가 같고, 내부에 크기가 그만큼 있다면
		if (Character == FinalBombChar && Stack.size() >= BombStrLength)
		{
			// 뒤에서부터 다 맞는지 확인한다.
			bool bIsCorrect = true;
			for (size_t i = 0; i < BombStrLength; ++i)
			{
				if (Stack[Stack.size()-1-i] != BombStr[BombStrLength - 1 - i])
				{
					bIsCorrect = false;
				}
			}

			if (bIsCorrect)
			{
				//문자열들을 모두 제거해주면됨
				auto it = Stack.end() - static_cast<int>(BombStrLength);
				Stack.erase(it, Stack.end());
			}
		}
	}

	if (Stack.empty())
	{
		std::cout<<"FRULA";
	}
	else
	{
		for (size_t i = 0; i < Stack.size(); ++i)
		{
			std::cout<<Stack[i];
		}
	}

}