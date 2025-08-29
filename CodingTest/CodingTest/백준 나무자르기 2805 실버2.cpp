#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

int N, M;
std::vector<long long> Trees;

bool DivideTree(long long Mid)
{
	long long MyTreeHeight = 0;
	for (size_t i = 0; i < Trees.size(); ++i)
	{
		if (Trees[i] <= Mid)
		{
			continue;
		}
		MyTreeHeight += ( Trees[i] - Mid);
	}
	return MyTreeHeight >= M;
}

int main()
{
	// 나무 M미터
	// H
	// 높이가 H보다 큰 나무는 H위 부분이 잘리고
	// 낮은 나무는 잘리지 않음

	// 20 15 10 17 이라하면
	// 높이 H 15라 잡을 시
	// 15 15 10 15 가 될 것이고
	// 길이가 5인나무와 2인 나무를 집에 들고감 (총 7미터)

	// 나무를 필요한 만큼만,
	// 적어도 M 미터 나무를 가져가려 할 때 최대 H

	std::cin.tie(nullptr);
	std::cin>>N>>M;
	Trees.resize(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>> Trees[i];
	}

	long long Left = 0;
	long long Right = *std::max_element(Trees.begin(), Trees.end());
	long long Answer = Right;

	

	while (Left <= Right)
	{
		long long Mid = (Left + Right) / 2;
		if (DivideTree(Mid))
		{
			Answer = Mid;
			Left = Mid + 1;
		}
		else
		{
			Right = Mid -1;
		}
	}
	std::cout << Answer;
}