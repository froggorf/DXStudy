#include <iostream>
#include <queue>
#include <vector>

int Board[1002][1002];
int Distance[1002][1002];
int X, Y;

int main()
{
	std::queue<std::pair<int,int>> Q;

	std::cin >> Y >> X;
	for(int i = 0 ;i < X; ++i)
	{
		for(int j = 0; j < Y; ++j)
		{
			int data;
			std::cin>>data;
			Board[i][j] = data;
			Distance[i][j] = -1;
			if(Board[i][j] == 1)
			{
				Q.emplace(i,j);
				Distance[i][j] = 0;
			}
		}
	}

	int dx[4] = {0,0,-1,1};
	int dy[4] = {1,-1,0,0};

	while(!Q.empty())
	{
		std::pair<int, int> Cur = Q.front(); Q.pop();
		for(int i = 0; i < 4; ++i)
		{
			int NextX = Cur.first + dx[i];
			int NextY = Cur.second + dy[i];

			if(NextX< 0 || NextX >= X || NextY < 0 || NextY >= Y)continue;
			if(Distance[NextX][NextY] != -1 || Board[NextX][NextY] == -1) continue;
			Distance[NextX][NextY] = Distance[Cur.first][Cur.second]+1;
			Q.emplace(NextX,NextY);
		}
	}

	int DayCount = 0;
	for(int i = 0; i < X; ++i)
	{
		for(int j = 0 ; j < Y; ++j)
		{
			if(Distance[i][j] == -1 && Board[i][j] == 0)
			{
				std::cout<<-1;
				return 0;
			}
			if(DayCount < Distance[i][j])
			{
				DayCount = Distance[i][j];
			}
		}
	}

	std::cout<<DayCount;

	
}