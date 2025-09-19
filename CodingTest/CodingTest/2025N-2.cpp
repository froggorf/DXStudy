#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <vector>

int NodeCount;
int EdgeCount;
std::vector<std::vector<int>> Graph;
// 0->3->0
bool IsCircularNode(int StartNode, int CurNode, int Depth, std::vector<bool>& Visited)
{
	Visited[CurNode] = true;
	const std::vector<int>& NextAdj = Graph[CurNode];
	for (int Next : NextAdj)
	{
		// 순환 구조인것
		if (Next == StartNode && Depth != 1)
		{
			return true;
		}

		if (!Visited[Next])
		{
			if (IsCircularNode(StartNode,Next, Depth+1, Visited))
			{
				return true;
			}
		}
	}

	return false;
}
int main()
{
	int s_nodes, s_edges;
	std::vector<int> s_from;
	std::vector<int> s_to;
	std::cin>>s_nodes;
	std::cin>>s_edges;

	NodeCount = s_nodes;
	EdgeCount = s_edges;
	Graph = std::vector<std::vector<int>>(NodeCount);
	for (int i = 0; i < EdgeCount; ++i)
	{
		int Val;
		std::cin>>Val;
		s_from.emplace_back(Val);
	}
	for (int i = 0; i < EdgeCount; ++i)
	{
		int Val;
		std::cin>>Val;
		s_to.emplace_back(Val);
	}

	// 노드와 선 존재
	// 두 노드를 연결하는 선은 한개만 존재
	// 순환 하나 있음
	// 어떤 노드는 그 순환으로부터 떨어져있고

	// 각 노드가 순환 구조로 구성된 내부 그래프로부터 얼마나 떨어져있는지
	// -> BFS 로 풀면 될것같음
	for (int i = 0; i < EdgeCount; ++i)
	{
		int Start = s_from[i];
		int End = s_to[i];
		Graph[Start].emplace_back(End);
		Graph[End].emplace_back(Start);
	}

	// 그렇다면... 일단 순환구조를 이루는 노드가 누구누구가 있는지 체크를 해보면
	std::set<int> CircularNodes;
	for (int i = 0; i < NodeCount; ++i)
	{
		std::vector<bool> Visited(NodeCount, false);
		if (IsCircularNode(i,i,0, Visited))
		{
			CircularNodes.emplace(i);
		}
	}

	// 여기까지 하면 순환인 애들을 찾음
	std::vector<int> Answer(NodeCount, 0);
	for (int i = 0; i < NodeCount; ++i)
	{
		std::queue<std::pair<int,int>> Q;
		std::vector<bool> Visited(NodeCount,false);
		Q.emplace(i,0);
		Visited[i] = true;
		while (!Q.empty())
		{
			std::pair<int,int> Cur = Q.front(); Q.pop();
			// 순환 노드들 중에 해당 Cur.first가 있다면
			if (std::find(CircularNodes.begin(), CircularNodes.end(), Cur.first) != CircularNodes.end())
			{
				Answer[i] = Cur.second;
				break;
			}

			const std::vector<int>& NextAdj = Graph[Cur.first];
			for (int Next : NextAdj)
			{
				if (!Visited[Next])
				{
					Visited[Next] = true;
					Q.emplace(Next, Cur.second+1);
				}
			}
		}
	}

	for (int Ans : Answer)
	{
		std::cout << Ans<< " ";
	}
}