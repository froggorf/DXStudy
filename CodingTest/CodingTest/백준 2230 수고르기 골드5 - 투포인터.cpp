#include <iostream>
#include <array>
#include <algorithm>
int N, M;
std::array<int, 100002> Arrs;
int main()
{
	std::cin.tie(0);
	std::cin>>N >>M;
	for(int i = 0 ; i < N; ++i)
	{
		std::cin>>Arrs[i];
	}

	std::sort(Arrs.begin(), Arrs.begin()+N);

	int answer = 0;

	
	int min = 0x7fffffff;
	auto start = Arrs.begin();
	auto end = Arrs.begin();
	while(true)
	{
		if(end==Arrs.end())
		{
			break;
		}

		int val = *end - *start;
		if(val < M)
		{
			++end;
		}
		else
		{
			if(val < min)
			{
				min = val;
			}
			++start;
		}
	}

	std::cout << min;
}
