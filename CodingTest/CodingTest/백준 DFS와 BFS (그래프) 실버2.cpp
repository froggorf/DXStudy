#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>
int N,M,V;
std::unordered_map<int, std::vector<int>> AdjMat;
std::vector<bool> DFS_Visited;
std::vector<bool> BFS_Visited;
void DFS(int InputNext)
{
	int Cur = InputNext;
	DFS_Visited[Cur] = true;
	std::cout << Cur << " ";

	const std::vector<int>& NextAdj = AdjMat[Cur];
	for (const int Next : NextAdj)
	{
		if (!DFS_Visited[Next])
		{
			DFS(Next);
		}
	}
}

void BFS(std::queue<int>& Q)
{
	if (Q.empty())
	{
		return;
	}

	int Cur = Q.front(); Q.pop();
	std::cout << Cur << " ";

	const std::vector<int>& NextAdj = AdjMat[Cur];
	for (const int Next : NextAdj)
	{
		if (!BFS_Visited[Next])
		{
			Q.emplace(Next);
			BFS_Visited[Next] = true;
			
		}
	}

	BFS(Q);
}

int main()
{
	
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cin>>N>>M>>V;
	AdjMat = std::unordered_map<int, std::vector<int>>(N+1);
	DFS_Visited = std::vector<bool>(N+1, false);
	BFS_Visited = std::vector<bool>(N+1, false);

	for (int i = 0; i < M; ++i)
	{
		int Start, End;
		std::cin>>Start>>End;
		AdjMat[Start].emplace_back(End);
		AdjMat[End].emplace_back(Start);
	}
	for (int i = 1; i < N+1; ++i)
	{
		std::sort(AdjMat[i].begin(), AdjMat[i].end()); 	
	}

	DFS(V);
	std::cout << "\n";

	std::queue<int> Q;
	Q.emplace(V);
	BFS_Visited[V] = true;
	BFS(Q);

}