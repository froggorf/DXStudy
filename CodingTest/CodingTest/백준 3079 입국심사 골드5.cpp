#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
int N,M;

bool Func(long long Mid, const std::vector<int>& Times)
{
	// Mid 라는 시간이 주어질 때
	long long CurPeopleCount = 0;
	for (const int Time : Times)
	{
		// 처리할 수 있는 사람 수
		CurPeopleCount += (Mid / Time);
		if (CurPeopleCount >= M)
		{
			return true;
		}
	}
	return CurPeopleCount >= M;
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	// 입국심ㄱ사대 N개
	// 심사 걸리는 시간 사람마다 다름,
	// k번 심사대에 심사관이 한명을 심사하는데 걸리는시간은
	// Tk

	// 두 심사대가 있고
	// 7 10초 사람이 6명이면
	// 7초에 3번째
	// 10초에 4번째
	// 14 5번쨰
	// 20초
	std::cin>>N>>M;
	std::vector<int> Times(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin >> Times[i];
	}

	//... 그리디.. 문제같은데..
	// 7*6 -> 42
	// 7*5 , 10*1 -> 35
	// 7*4 , 10*2 -> 28
	// 7*3 , 10*3 -> 30
	// 7*2, 10*4 -> 40
	// 모든 걸 곱했을 때 평균값으로 만드는 그 무언가를 찾으면 될것같은데..
	// 그리디 + 이분탐색...? ㅋㅋ

	// 맥스값을 일단 가져가면 될것같음..
	//

	// 모두다 for loop 로 시뮬레이션을 돌린다...라고 생각했을 때,,,
	// 시간이... 좋진 않을것같음
	// 지금 생각해보니까
	// 1분일때 각 애들이 보낼 수 있는 사람들 모두 더한거
	// 2분일 때...
	// n분일 때 처리할 수 있는거 다 구한다음에
	// 그거의 최소치 시간을 구하면 되지않을까??

	long long Left = 0;
	long long MaxElement = *std::max_element(Times.begin(),Times.end());
	long long Right = MaxElement * M;
	long long Answer = Right;

	while (Left <= Right)
	{
		long long Mid = (Left + Right) / 2;
		// 사람수 M명이 해당 Mid 분 안에 채워지면
		if (Func(Mid, Times))
		{
			Right = Mid - 1;
			Answer = std::min(Mid,Answer);
		}
		else
		{
			Left = Mid + 1;
		}

	
	}
	std::cout<<Answer;
}