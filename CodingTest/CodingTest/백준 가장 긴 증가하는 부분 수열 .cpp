#include <iostream>
#include <vector>

int main()
{
	int N;
	std::cin>>N;
	std::vector<int> Values(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>> Values[i];
	}

	// DP[i] -> i를 마지막으로 하는 가장 긴 수열의 길이
	std::vector<int> DP(N,1);

	int Answer = 1;
	for (int Target = 1; Target < N; ++Target)
	{
		for (int i = 0; i < Target; ++i)
		{
			if (Values[i] < Values[Target])
			{
				DP[Target] = std::max(DP[Target], DP[i] + 1);
				Answer = std::max(Answer, DP[Target]);
			}
		}
	}
	std::cout<<Answer;
}
