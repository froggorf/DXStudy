#include <iostream>
#include <queue>
#include <vector>

int N, M;
int Board[500][500];
int Visit[500][500];

int PicCount = 0;
std::vector<int> PicSizes;

int dx[4] = {1,-1,0,0};
int dy[4] = {0,0,1,-1};

void BFS()
{
	
}


int main()
{
	std::cin>>N>>M;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0 ; j < M; ++j)
		{
			std::cin >> Board[i][j];
			if(Board[i][j] == 0)
			{
				Visit[i][j] = true;
			}
		}
	}

	std::queue<std::pair<int,int>> Queue;
	
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j < M ;  ++j)
		{
			// 방문한 적 없음 && 그림이 있음
			if(Visit[i][j] == false && Board[i][j] == 1)
			{

				++PicCount;
				int PicSize = 1;

				Visit[i][j] = true;
				Queue.emplace(i,j);

				while (!Queue.empty())
				{
					std::pair<int,int> Cur = Queue.front(); Queue.pop();
					for(int dir = 0; dir < 4; ++dir)
					{
						int NextX = Cur.first + dx[dir];
						int NextY = Cur.second + dy[dir];
						// 경계를 넘어감
						if(NextX < 0 || NextX >= N || NextY < 0 || NextY >= M) continue;

						// 방문했었거나 || 그림이 없으면 중단
						if(Visit[NextX][NextY] || Board[NextX][NextY] != 1) continue;
						++PicSize;
						Visit[NextX][NextY] = true;
						Queue.emplace(NextX,NextY);
					}
				}
				PicSizes.emplace_back(PicSize);
			}
		}
	}

	std::cout<<PicCount<<std::endl;
	if(PicCount == 0)
	{
		std::cout<<0<<std::endl;
		return 0;
	}

	int Large = 0;
	for(const auto& PicSize : PicSizes)
	{
		if(Large < PicSize)
		{
			Large= PicSize;
		}
	}
	std::cout<<Large;
	


}