#include <iostream>
#include <vector>
#include <string>
#include <valarray>

int Check(const std::string& Str, std::string::iterator Begin, std::string::iterator End, bool Pseudo)
{
	while (Begin < End)
	{
		if (*Begin == *End)
		{
			Begin += 1;
			End -= 1;
		}
		else
		{
			if (!Pseudo)
			{
				if (Check(Str, Begin+1, End, true) == 0)
				{
					return 1;				
				}
				else if (Check(Str, Begin, End - 1, true) == 0)
				{
					return 1;
				}
			}

			return 2;
		}
	}

	return 0;
}

int main()
{
	int N;
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cin>>N;
	for (int i = 0; i < N; ++i)
	{
		std::string S;
		std::cin >> S;
		auto Begin = S.begin();
		auto End = S.end()-1;

		bool Pseudo = false;
		int Answer = Check(S, Begin, End, Pseudo);
		std::cout << Answer << "\n";
	}
	
}
