#include <iostream>
#include <vector>

int N,M;
std::vector<std::vector<int>> Pre;
int main()
{
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	std::cin>>N>>M;

	Pre = std::vector<std::vector<int>>(N+1);
	for (int i = 0; i < M; ++i)
	{
		int Left, Right;
		std::cin >> Left >> Right;
		Pre[Right].emplace_back(Left);
		//Pre[2] = {1}
		//Pre[3] = {2}
	}

	// 즉,
	std::vector<int> DP(N+1,1);
	for (size_t i = 1; i < DP.size(); ++i)
	{
		const std::vector<int>& Prerequisite = Pre[i];
		for (const int PrerequisiteNum : Prerequisite)
		{
			DP[i] = std::max(DP[i], DP[PrerequisiteNum]+1);
		}
		std::cout<<DP[i]<<" ";
	}
	
}