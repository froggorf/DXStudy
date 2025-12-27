#include <iostream>
#include <vector>
#include <algorithm>
int main()
{
	int N;
	std::cin>>N;
	std::vector<int> Nums(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>>Nums[i];
	}
	std::vector<int> DP(N);

	for (int i = 0; i < N; ++i)
	{
		DP[i] = 1;

		for (int j = 0; j < i; ++j)
		{
			if (Nums[j] < Nums[i])
			{
				DP[i] = std::max(DP[i], DP[j]+1);
			}
		}
	}

	std::cout << *std::max_element(DP.begin(), DP.end());
}