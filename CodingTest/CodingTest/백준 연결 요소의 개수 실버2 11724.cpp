#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
int main()
{
	int N, M;
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	std::cin >> N >> M;


	std::unordered_map<int, std::vector<int>> Data(N + 1);
	for (int i = 0; i < M; ++i)
	{
		int Start, End;
		std::cin >> Start >> End;
		Data[Start].emplace_back(End);
		Data[End].emplace_back(Start);
	}

	std::vector<bool> bVisited(N+1, false);
	std::stack<int> Q;

	int CurComponentCount = 0;
	for (int i = 1; i < N+1; ++i)
	{
		if (bVisited[i])
		{
			continue;
		}
		++CurComponentCount;
		Q.emplace(i);
		

		while (!Q.empty())
		{
			int Cur = Q.top(); Q.pop();
			bVisited[Cur] = true;
			const std::vector<int>& Next = Data[Cur];
			for (const int NextNum : Next)
			{
				if (!bVisited[NextNum])
				{
					Q.emplace(NextNum);
				}
				
			}
		}
	}

	std::cout << CurComponentCount;


}