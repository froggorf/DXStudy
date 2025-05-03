#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

std::array<long long, 100002> Arr;
int N, M;


int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::cin >> N >> M;
	for(int i = 0; i < N; ++i)
	{
		std::cin>>Arr[i];
	}
	std::sort(Arr.begin(), Arr.begin()+N);

	auto Fwd = Arr.begin();
	auto Bwd = Arr.begin()+1;
	long long Ans = 2e9;
	while(Fwd != Arr.end() && Bwd != Arr.end())
	{
		if(*Bwd - *Fwd < M)
		{
			++Bwd;
		}
		else
		{
			Ans = std::min(Ans, (long long)(*Bwd - *Fwd));
			++Fwd;
			if (Fwd == Bwd)
			{
				++Bwd;
			}
		}
	}
	std::cout << Ans;
}