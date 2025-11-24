// 2시 시작
#include <iostream>
#include <vector>
#include <queue>
int main()
{
	// 아하 이거 그건데... 그 위상정렬...
	// 이게 아마도
	// 일단 이어지는 애들을 다 만든다음에...
	int N, M;
	std::cin>> N >> M;
	std::vector<std::vector<int>> Graph(N);
	std::vector<int> InCount(N, 0);
	// 위상정렬을 보고오니, 자신에게 들어오는 간선이 없는 애들을 위주로 큐에 넣으면 되는거엿음
	// 그렇다면, 저장하는 방식 자체를 자신에게 
	for (int i = 0; i < M; ++i)
	{
		int Left, Right;
		std::cin>>Left >> Right;
		Left-=1;
		Right-=1;
		Graph[Left].emplace_back(Right);
		InCount[Right] += 1;
	}

	std::vector<int> Answer;
	std::queue<int> Q;
	for (int i = 0; i < N; ++i)
	{
		if (InCount[i] == 0)
		{
			Q.emplace(i);
		}
	}

	while (!Q.empty())
	{
		int Cur = Q.front(); Q.pop();
		// 위상정렬에 추가해주고서
		Answer.emplace_back(Cur);

		// 자신에게 이어지는 노드들의 InCount 를 줄여준다
		for (int Next : Graph[Cur])
		{
			--InCount[Next];
			if (InCount[Next] == 0)
			{
				Q.emplace(Next);
			}
		}
	}

	for (int i = 0; i < N; ++i)
	{
		std::cout<<Answer[i] + 1 <<" ";
	}
}