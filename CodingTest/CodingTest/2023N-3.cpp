#include <algorithm>
#include <vector>
#include <iostream>
#include <list>
#include <set>

int main()
{
	// 좌표대로 점/선분
	// 불필요한 선들이 보임
	// 최소한의 노력으로 선긋기

	// {15,15} {7,7} {2,3}, {6,11} {1,2}
	// -> 1,2랑 2,3 은 합쳐서 1,3으로 그리기 가능
	// {7,7}은 선분 {6,11} 에 포함

	int N;
	std::cin >> N;
	std::vector<std::pair<int,int>> Lines;
	for (int i = 0; i < N; ++i)
	{
		int Start, End;
		std::cin>>Start>>End;
		Lines.emplace_back(Start,End);
	}

	// 정렬 먼저 해주고
	std::sort(Lines.begin(), Lines.end(), [](const std::pair<int,int>& A, const std::pair<int,int>& B)
	{
		if (A.first == B.first)
		{
			return A.second < B.second;	
		}
		
		return A.first < B.first;
	});


	

	while (true)
	{
		bool AllNotIntersect = true;
		for (int i = 0; i < Lines.size()-1;)
		{
			//1,5 
			std::pair<int,int>& Left = Lines[i];
			// 2,3
			const std::pair<int,int>& Right = Lines[i+1];

			// 겹치지않음
			if (Left.first < Right.first && Left.second < Right.first)
			{
				++i;
			}
			// 겹침
			else
			{
				Left.first = std::min(Left.first,Right.first);
				Left.second = std::max(Left.second, Right.second);
				Lines.erase(Lines.begin() + i+1);
				AllNotIntersect = false;
			}
		}

		if (AllNotIntersect)
		{
			break;
		}
	}

	for (int i = 0; i < Lines.size(); ++i)
	{
		std::cout<< "[" << Lines[i].first<<", "<<Lines[i].second<<"]"<<", ";
	}

	
}

/*
5
6 9
2 3
9 11
1 5
14 18
 
 */