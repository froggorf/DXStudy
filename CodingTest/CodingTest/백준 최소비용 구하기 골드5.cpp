#include <iostream>
#include <queue>
#include <vector>

int main()
{
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	int N, M;
	std::cin>>N>>M;
	std::vector<std::vector<std::pair<int,int>>> Costs(N+1);
	for (int i = 0; i < M; ++i)
	{
		int Start, End, Cost;
		std::cin >> Start >> End >> Cost;
		Costs[Start].emplace_back(End,Cost);
	}
	int A, B;
	std::cin>>A>>B;

	std::vector<int> Dist(N+1, 0x0fff'ffff);
	// A에서 출발하니까 Dist[A] = 0;
	Dist[A] = 0;

	//std::queue<int> Q;
	std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<>> PQ;
	PQ.emplace(0,A);

	while (!Q.empty())
	{
		int Cur = Q.front(); Q.pop();

		const std::vector<std::pair<int,int>>& BusData = Costs[Cur];
		for (const std::pair<int,int>& Bus : BusData)
		{
			int NewCost = Dist[Cur] + Bus.second;
			if (NewCost < Dist[Bus.first])
			{
				Dist[Bus.first] = NewCost;
				Q.emplace(Bus.first);
			}
		}
	}
	std::cout<<Dist[B];
}


// 한 도시에서 출발
// 다른 도시 도착하는 M개의 버스
// A번째 도시에서 B번째 도시까지 가는데 드는 최소비용