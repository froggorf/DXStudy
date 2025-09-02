#include <iostream>
#include <vector>

int N, K;
std::vector<int> Weights;
std::vector<int> Values;

int main()
{
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	std::cin>>N>>K;
	Weights = std::vector<int>(N+1);
	Values = std::vector<int>(N+1);
	for (int i = 1; i < N+1; ++i)
	{
		std::cin>>Weights[i] >> Values[i];
	}

	// 일단, 0개 담았을때, 1개 담았을때, ... n개 담았을때 이런식으로
	// [x][y], x-> 몇개의 물건을 담았는지
	// y-> 그 무게의 최대 가치
	std::vector<std::vector<int>> DP(N+1, std::vector<int>(K+1));
	for (int i = 1; i < N+1; ++i)
	{
		for (int w = 1; w < K+1; ++w)
		{
			// 이 안에서,,
			// 이전 결과를 활용해야하는거잖아?
			// DP..
			// 만약 지금 w 무게가 weights[i]를 빼도 남으면
			if (w >= Weights[i])
			{
				DP[i][w] = std::max(DP[i-1][w-Weights[i]] + Values[i], DP[i-1][w]);	
			}
			else
			{
				DP[i][w] = DP[i-1][w];
			}

			
		}
		
	}

	std::cout << DP[N][K];
}