#include <algorithm>
#include <assert.h>
#include <iostream>
#include <queue>
#include <vector>

std::vector<std::vector<int>> Friend;
std::vector<int> Answers;

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	int N;
	std::cin>>N;
	Friend = std::vector<std::vector<int>>(N+1);
	Answers = std::vector<int>(N+1, 1);

	// 🔧 수정 1: 입력 처리
	while (true)
	{
		int First, Second;
		std::cin >> First >> Second;
		if (First == -1 && Second == -1) break;

		Friend[First].emplace_back(Second);
		Friend[Second].emplace_back(First);	
	}

	int MinAnswer = 0x0fff'ffff;
	for (int i = 1; i < N+1; ++i)
	{
		// 🔧 수정 2: 방문 체크 배열로 변경
		std::vector<bool> visited(N+1, false);
		visited[i] = true; // 시작점 방문 처리
		int visitedCount = 1;

		std::queue<std::pair<int,int>> Q;
		for (int F : Friend[i])
		{
			if (!visited[F])
			{
				visited[F] = true;
				visitedCount++;
				Q.emplace(1, F);
			}
		}

		while (visitedCount < N)
		{
			if (Q.empty()) assert(nullptr);

			std::pair<int,int> Front = Q.front(); Q.pop();
			int Depth = Front.first + 1;

			for (int FrontsFriend : Friend[Front.second])
			{
				// 🔧 수정 3: 중복 방지
				if (!visited[FrontsFriend])
				{
					visited[FrontsFriend] = true;
					visitedCount++;

					if (visitedCount == N)
					{
						MinAnswer = std::min(MinAnswer, Depth);
						Answers[i] = Depth;
						break;
					}

					Q.emplace(Depth, FrontsFriend);
				}
			}
		}
	}

	int Count = std::count(Answers.begin(),Answers.end(), MinAnswer);
	std::cout << MinAnswer << " " << Count <<"\n";
	for (int i = 1;  i < N+1; ++i)
	{
		if (MinAnswer == Answers[i])
		{
			std::cout<<i << " ";	
		}
	}
}