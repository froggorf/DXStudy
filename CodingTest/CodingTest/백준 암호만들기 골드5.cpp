#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

int main()
{
	// L개의 알파벳 소문자
	// 한개의 모음
	// 두개의 자음으로 구성
	// 오름차순

	// C가지의 가능성

	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);

	int L, C;
	std::cin>>L>>C;
	std::vector<char> Data(C);
	for (int i = 0; i < C; ++i)
	{
		std::cin >> Data[i];
	}
	// C개중에 L개를 골라서
	// 가능한것들을 사전순으로 출력 -> next_permutation.
	
	std::sort(Data.begin(), Data.end());

	std::vector<int> Permutation(C,0);
	for (int i = 0; i < L; ++i)
	{
		Permutation[C-1-i] = 1;
	}

	std::vector<std::string> Answers;

	do
	{
		std::string Str = "";
		Str.reserve(C);
		for (int i =0 ; i < C; ++i)
		{
			if (Permutation[i])
			{
				Str = Str + Data[i];
			}
		}

		int AEIOUCount = 0;
		int ElseCount = 0;
		bool IsInsertStr = false;
		for (int i = 0; i < L; ++i)
		{
			if (Str[i] == 'a' || Str[i] == 'e' || Str[i] == 'i' || Str[i] == 'o' || Str[i] == 'u')
			{
				++AEIOUCount;
			}
			else
			{
				++ElseCount;
			}
			IsInsertStr = AEIOUCount>=1 && ElseCount >= 2;
			if (IsInsertStr)
			{
				break;
			}
		}

		if (IsInsertStr)
		{
			Answers.emplace_back(Str);	
		}
		
	}while (std::next_permutation(Permutation.begin(),Permutation.end()));

	for (const auto& P : Answers)
	{
		std::cout<<P<<"\n";
	}
}
