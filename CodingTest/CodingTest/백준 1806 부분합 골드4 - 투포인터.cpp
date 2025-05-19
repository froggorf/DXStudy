#include <iostream>
#include <array>
#include <algorithm>

int N,S;
std::array<int, 100002> Arrs;
int main()
{
	std::cin.tie();
	std::cin >> N >> S;
	for(int i = 0; i < N; ++i)
	{
		std::cin>>Arrs[i];
	}

	auto start = Arrs.begin();
	auto end = Arrs.begin();
	int CurVal = *start;
	unsigned int min = 0xffffffff;
	while(true)
	{
		if(CurVal >= S)
		{
			if(min > end - start)
			{
				min = end - start;
			}
			CurVal -= *start;
			++start;
		}
		else
		{
			++end;
			if(end == Arrs.end())
			{
				break;
			}
			CurVal += *end;
		}
	}
	if(min == 0xffffffff)
	{
		std::cout<<0;
		return 0;
	}
	std::cout << min+1;
}