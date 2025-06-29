#include <iostream>
#include <queue>
#include <set>
#include <vector>

int Board[102][102];
bool bIsVisited[102][102];

int main()
{
	std::cin.tie(0);
	int M,N,K;
	std::cin>>M>>N>>K;

	// 그림 그리기
	for (int i = 0; i < K; ++i)
	{
		int Left, Top, Right, Bottom;
		std::cin>> Left >> Bottom >> Right >> Top;
		for (int x = Bottom; x < Top; ++x)
		{
			for (int y = Left; y < Right; ++y)
			{
				Board[x][y] = 1;
			}
		}
	}

	std::multiset<int> Answer;

	int dx[4] = {0,0,1,-1};
	int dy[4] = {1,-1,0,0};
	// 모든 칸에 대해서 돌면서 빈칸 체크
	for (int x = 0; x < M; ++x)
	{
		for (int y = 0; y < N; ++y)
		{
			std::queue<std::pair<int, int>> Q;
			if (Board[x][y] == 0 && bIsVisited[x][y] == false)
			{
				Q.emplace(x,y);
				bIsVisited[x][y] = true;
			}

			

			int CurSize = 0;
			while (!Q.empty())
			{
				++CurSize;
				std::pair<int,int> Cur = Q.front(); Q.pop();
				for (int dir = 0; dir<4; ++dir)
				{
					int NextX = Cur.first + dx[dir];
					int NextY = Cur.second + dy[dir];
					if (NextX < 0 || NextX >= M || NextY < 0 || NextY >= N) continue;
					if (Board[NextX][NextY] == 1 || bIsVisited[NextX][NextY]) continue;
					bIsVisited[NextX][NextY] = true;
					Q.emplace(NextX,NextY);
				}
			}
			if (CurSize > 0)
			{
				Answer.emplace(CurSize);

			}
		}
	}

	std::cout<<Answer.size()<<"\n";
	for (auto p = Answer.begin(); p!=Answer.end(); ++p)
	{
		std::cout<< *p << " ";
	}

		
}