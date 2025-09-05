#include <iostream>
#include <list>
#include <queue>
#include <vector>

int main()
{
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);

    int N, M;
    std::cin >> N >> M;

    // 인접 리스트: [출발도시][i] = {도착도시, 비용}
    std::vector<std::vector<std::pair<int, int>>> graph(N + 1);

    for (int i = 0; i < M; ++i)
    {
        int start, end, cost;
        std::cin >> start >> end >> cost;
        graph[start].emplace_back(end, cost);
    }

    int A, B;
    std::cin >> A >> B;

    // 최단 거리 배열 (무한대로 초기화)
    std::vector<int> dist(N + 1, 0x3f3f3f3f);
    dist[A] = 0;

    // 우선순위 큐 (최소 힙): {비용, 도시번호}
    std::priority_queue<std::pair<int, int>, 
        std::vector<std::pair<int, int>>, 
        std::greater<std::pair<int, int>>> pq;
    
    pq.emplace(0, 1);
    while (!pq.empty())
    {
        auto [currentCost, currentCity] = pq.top();
        pq.pop();

        // 이미 더 좋은 경로가 있으면 스킵
        if (currentCost > dist[currentCity]) continue;

        // 목표 도시에 도달하면 조기 종료
        if (currentCity == B) break;

        // 인접한 도시들 탐색
        for (const auto& [nextCity, busCost] : graph[currentCity])
        {
            int newCost = currentCost + busCost;
            if (newCost < dist[nextCity])
            {
                dist[nextCity] = newCost;
                pq.emplace(newCost, nextCity);
            }
        }
    }

    std::cout << dist[B] << '\n';

    return 0;
}