#include <iostream>
#include <queue>
#include <vector>
using namespace std;


int N, M;
int Board[100][100];
int Distance[100][100];

int dx[4] = {0,0,-1,1};
int dy[4] = {1,-1,0,0};

int main()
{
	std::cin >> N >> M;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0 ; j < M ; ++j)
		{
			char dummy;
			std::cin>>dummy;
			Board[i][j] = dummy - '0';
		}
	}
	for(int i = 0; i < N ; ++i)
	{
		std::fill_n(Distance[i], M, -1);
	}


	queue<pair<int,int>> Queue;
	Distance[0][0] = 1;
	Queue.emplace(0,0);

	
	while(!Queue.empty())
	{
		pair<int,int> Cur = Queue.front(); Queue.pop();
		if(Cur.first == N-1 && Cur.second == M-1)
		{
			break;
		}

		for(int dir = 0; dir < 4; ++dir)
		{
			int NextX = Cur.first + dx[dir];
			int NextY = Cur.second+ dy[dir];
			if(NextX<0 || NextX>=N || NextY < 0 ||NextY >=M)continue;
			if(Distance[NextX][NextY] != -1 || Board[NextX][NextY] == 0) continue;
			Distance[NextX][NextY]= Distance[Cur.first][Cur.second]+1;
			Queue.emplace(NextX,NextY);
		}
	}
	std::cout<<Distance[N-1][M-1];

	
}