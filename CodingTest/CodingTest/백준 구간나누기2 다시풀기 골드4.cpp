#include <iostream>
#include <vector>
#include <algorithm>

int N,M;
bool Func(const std::vector<int>& Nums, int Mid)
{
	int CurCount = 1;
	int CurMaxVal = Nums[0];
	int CurMinVal = Nums[0];
	for (size_t i = 0; i < Nums.size(); ++i)
	{
		CurMaxVal = std::max(CurMaxVal, Nums[i]);
		CurMinVal = std::min(CurMinVal, Nums[i]);
		if (CurMaxVal - CurMinVal > Mid)
		{
			++CurCount;
			CurMaxVal = Nums[i];
			CurMinVal = Nums[i];
		}
	}
	return CurCount <= M;
}

int main()
{
	// 점수의 최댓값을 최소로 -> 그리디 이분탐색
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);

	std::cin>>N>>M;
	std::vector<int> Nums(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin >> Nums[i];
	}

	int Left = 0;
	int Right = *std::max_element(Nums.begin(), Nums.end()) - *std::min_element(Nums.begin(),Nums.end());;
	int Answer = Right;

	while (Left <= Right)
	{
		int Mid = (Left + Right) / 2;
		if (Func(Nums, Mid))
		{
			// 구간을 나눌 수 있으면
			Right = Mid - 1;
			Answer = Mid;
		}
		else
		{
			// 구간 못 나누면
			Left = Mid + 1;
			
		}
	}

	std::cout << Answer;
}