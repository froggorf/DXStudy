#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

int main()
{
	// ZOAC 면은
	// A AC OAC ZOAC 순으로 보여주는거잖아..
	// 일단 그러면
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);


	std::string S;
	std::cin>>S;

	// CABCACB
	// A
	// AA
	//
	int Count = 0;
	std::vector<bool> IsInput(S.size(),false);
	while (Count < S.length())
	{
		++Count;
		std::string FastestS = "a";
		int FastestIndex = -1;
		for (int i = 0; i < S.size(); ++i)
		{
			if (IsInput[i]) continue;

			std::string Test;
			Test.reserve(Count);
			for (int j = 0; j < S.size(); ++j)
			{
				if (j==i || IsInput[j])
				{
					Test = Test+S[j];
				}
			}

			if (FastestS > Test)
			{
				FastestS = Test;
				FastestIndex = i;
			}
		}
		std::cout<<FastestS<<"\n";
		IsInput[FastestIndex] = true;
	}
}
