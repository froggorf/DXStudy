#include <iostream>
#include <queue>
#include <utility>
#include <vector>

bool IsInDistance(const std::pair<int,int>& A, const std::pair<int,int>& B)
{
	return std::abs(A.first - B.first) + std::abs(A.second- B.second) <= 1000;
}

int main()
{
	// 맥주 한병 마셔야 50미터를 갈 수 있음
	// 박스에 최대 20병만 들어감

	// 편의점, 상근이집, 페스티벌 좌표

	std::cin.tie(nullptr);
	int T;
	
	std::cin>>T;

	for (int ActiveCount = 0 ; ActiveCount < T; ++ActiveCount)
	{
		int ConvCount = 0;
		std::cin >> ConvCount;
		std::vector<std::pair<int,int>> Conv(ConvCount + 1);
		std::cin >> Conv[0].first >> Conv[0].second;
		for (int i = 1; i <ConvCount+1; ++i)
		{
			std::cin >> Conv[i].first >> Conv[i].second;
		}
		std::pair<int,int> Fest;
		std::cin>>Fest.first >> Fest.second;

		std::vector<bool> Visited(ConvCount+1, false);

		std::queue<int> Q;
		Q.emplace(0);
		bool bGoneFest = false;
		// 현재 큐에 담긴것이 없거나 현재 갈 수 있는 편의점이 없을 경우,
		while (!Q.empty())
		{
			int NewConv = Q.front(); Q.pop();
			Visited[NewConv] = true;

			if (IsInDistance(Fest, Conv[NewConv]))
			{
				bGoneFest = true;
				break;
			}

			for (int i = 1; i < ConvCount + 1; ++i)
			{
				if (Visited[i]) continue;

				if (IsInDistance(Conv[i], Conv[NewConv]))
				{
					Q.emplace(i);
				} 
			}
		}
		if (bGoneFest)
		{
			std::cout << "happy"<<"\n";
		}
		else
		{
			std::cout << "sad"<<"\n";
		}
	}
	
}