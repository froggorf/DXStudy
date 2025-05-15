#include <iostream>
#include <array>
#include <algorithm>
#include <vector>


int main()
{
	int N;
	std::cin>>N;

	std::vector<std::pair<int,int>> Times(N);
	for(int i = 0; i < N; ++i)
	{
		std::cin>>Times[i].first >> Times[i].second;
	}

	std::sort(Times.begin(), Times.end(), [](const std::pair<int,int>& A, const std::pair<int,int>& B)
	{
		if(A.second == B.second)
		{
			return A.first < B.first;
		}
		return A.second < B.second;
	});

	int CurEndTime = 0;
	int Answer = 0;
	for(int i = 0; i < N; ++i)
	{
		if(Times[i].first >= CurEndTime)
		{
			++Answer;
			CurEndTime = Times[i].second;
		}
	}
	std::cout<<Answer;
}
