#include <iostream>
#include <vector>

int main()
{
	// 음수가 있는 사이클은 벨만-포드 알고리즘
	// 즉 노드 N개 일때 N-1 번 엣지를 돌면서 갱신한다음에
	// 한번 더 갱신이 되면 그땐 음수사이클이 발생한다는것
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	int N,M;
	std::cin>>N >> M;
	std::vector<std::vector<std::pair<int,int>>> Bus(N+1);
	for (int i = 0; i < M; ++i)
	{
		int Start,End,Weight;
		std::cin>>Start>>End>>Weight;
		Bus[Start].emplace_back(End,Weight);
	}

	std::vector<long long> Dist(N+1, INT32_MAX);
	Dist[1] = 0;
	for (int i = 0; i < N-1; ++i)
	{
		for (size_t Start = 1; Start < Bus.size(); ++Start)
		{
			for (const std::pair<int,int>& BusInfo : Bus[Start])
			{
				int End = BusInfo.first;
				int Weight = BusInfo.second;
				// Start 까지 가는 경로를 찾지 못했으면, 굳이 뒤에를 볼 필요가 없기에
				if (Dist[Start] != INT32_MAX && Dist[End] > Dist[Start] + Weight)
				{
					// 1에서 End로 가는 시간이
					// 1에서 Start로 가서 Weight 가는 시간보다 길면 갱신
					Dist[End] = Dist[Start]+Weight;
				}
			}
		}
	}


	for (size_t Start = 1; Start < Bus.size(); ++Start)
	{
		for (const std::pair<int,int>& BusInfo : Bus[Start])
		{
			int End = BusInfo.first;
			int Weight = BusInfo.second;
			// Start 까지 가는 경로를 찾지 못했으면, 굳이 뒤에를 볼 필요가 없기에
			if (Dist[Start] != INT32_MAX && Dist[End] > Dist[Start] + Weight)
			{
				// 만약 n번째 돌았는데 한번 더 발생한다? 그럼 음수사이클
				std::cout<<-1;
				return 0;
			}
		}
	}

	for (int i = 2; i <= N; ++i)
	{
		if (Dist[i] == INT32_MAX)
		{
			std::cout<<-1<<"\n";
		}
		else
		{
			std::cout<<Dist[i]<<"\n";
		}
	}

}