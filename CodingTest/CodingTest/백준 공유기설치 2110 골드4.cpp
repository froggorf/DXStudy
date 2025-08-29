#include <algorithm>
#include <iostream>
#include <vector>

int N, C;
std::vector<int> House;
int main()
{
	std::cin.tie(nullptr);
	std::cin>>N>>C;
	House = std::vector<int>(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>> House[i];	
	}
	
	// 집개수 N
	// 공유기개수 C

	// 한집에는 공유기를 하나만 설치
	// 가장 인접한 두 공유기 사이를 가능한 크게
	// C개의 공유기를 N개의 집에 적당히 설치해서
	// 가장 인접한 두 공유기 사이의 거리를 최대로..

	// 1,2,8,4,9
	// 1 2 4 8 9
	// 공유기 3개가 주어질 때,

	// 1 4 8에 설치하면
	// 가장 인접한 건 3
	// 1 4 9 에 설치하면 가장 인접한건 3

	// 즉, 가장 넓게 설치할 수 있는 위치를 찾으면 되는거니까..
	// 일단 
	// C 개의 지점을 찾아야하고,
	// 그 각 지점간의 거리를 최대치로 할 수 있게 해야함

	// 왜 이분탐색일까..
	// 가능한 크게 -> 그리디
	// 그 그리디를 활용하는 것에서 최대값을 찾는 문제이므로 이분탐색을 해야한다
	// 라는 의미인것 같음.

	// 그럼 접근을.
	// 일단은 공유기의 최대 거리로 가능한 걸 보자,
	// 정렬을 한다음에
	//std::sort(House.begin(), House.end());
	//int FarHouseIndex = static_cast<int>(House.size())-1;
	// 한 공유기가 최소로 커버할 공간은
	//float Value = static_cast<float>(FarHouseIndex) / C;
	//int IndexGap = static_cast<int>(std::ceil(Value));
	// 가 될 것임
	// 예제에서 5개의 개수를 3개로 커버하니까,
	// 인덱스는 최대 2개를 건너뛰면서 설치가 가능한거지,

	/*
	1 2 4 8 9
	4에다가 설치를 해
	그다음에 왼쪽을 봐,
	왼쪽에서 
	*/


	// 일단 그러면,
	std::sort(House.begin(), House.end());
	int MaxElement = 0xffff'ffff;
	int MinElement = 0x0fff'ffff;
	for (size_t i = 0; i < House.size(); ++i)
	{
		MaxElement = std::max(MaxElement, House[i]);
		MinElement = std::min(MinElement, House[i]);
	}
	//int Gap = (MaxElement-MinElement) / C;

	auto CanWifi = [](int Mid)->bool
	{
		int RetAnswer = 1;
		auto LeftIter = House.begin();
		for (auto Iter = House.begin()+1; Iter != House.end(); ++Iter)
		{
			// 공유기 하나 더 설치해야함
			if (*Iter - *LeftIter >= Mid)
			{
				++RetAnswer;
				LeftIter = Iter;
			}
		}
		
		return RetAnswer >= C;
	};

	int Left = 1;
	int Right = House[N-1] - House[0] + 1;
	int Answer = House[N-1] - House[0] + 1;
	while (Left <= Right)
	{
		int Mid = (Left + Right) / 2;
		if (CanWifi(Mid))
		{
			Left = Mid+1;
			Answer = Mid;
		}
		else
		{
			Right = Mid-1;
		}
	}
	std::cout<<Answer;

	
	


}