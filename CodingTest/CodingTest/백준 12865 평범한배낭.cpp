#include <iostream>
#include <vector>

int N;
int W;
std::vector<int> Weights;
std::vector<int> Values;
int main()
{
	std::cin >> N >> W;
	Weights.resize(N+1);
	Values.resize(N+1);
	for (int i = 1; i < N+1; ++i)
	{
		std::cin>> Weights[i] >> Values[i];
	}

	std::vector<std::vector<int>> DP;
	DP.resize(N+1);
	for (size_t i = 0; i < DP.size(); ++i)
	{
		DP[i].resize(W+1);
		for (size_t j = 0; j < W+1; ++j)
		{
			DP[i][j] = 0;
		}
	}

	for (int i = 1; i < N+1; ++i)
	{
		for (int w = 0; w < W+1; ++w)
		{
			// 무게가 안돼서 못넣는것
			if (w < Weights[i])
			{
				DP[i][w] = DP[i-1][w];
			}
			// 넣을 수 있는것
			else
			{
				DP[i][w] = std::max(
					DP[i-1][w],
					DP[i-1][w-Weights[i]] + Values[i]
				);
			}
		}
	}

	std::cout << DP[N][W];
}