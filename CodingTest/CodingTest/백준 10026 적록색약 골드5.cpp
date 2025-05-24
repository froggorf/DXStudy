#include <iostream>
#include <queue>
#include <array>
int N;
std::array<std::array<char,102>,102> Picture;
std::array<std::array<bool,102>,102> RGYesVisited;
std::array<std::array<bool,102>,102> RGNoVisited;
std::queue<std::pair<int,int>> RGYesQ;
std::queue<std::pair<int,int>> RGNoQ;
int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::cin>>N;

	
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0 ; j < N; ++j)
		{
			char RGB;
			std::cin>>RGB;
			Picture[i][j] = RGB;
		}
	}

	int dx[4] = {0,0,1,-1};
	int dy[4] = {1,-1,0,0};
	// 색약 아닌사람이 보는것 먼저 구하기
	int RedGreenYes = 0;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j< N; ++j)
		{
			// 방문한적 있으면 남긴거니까 다음으로 패스
			if(RGYesVisited[i][j])
			{
				continue;
			}
			
			RGYesQ.emplace(i,j);
			
			while(!RGYesQ.empty())
			{
				std::pair<int,int> Cur = RGYesQ.front(); RGYesQ.pop();
				for(int dir = 0; dir < 4; ++dir)
				{
					int NextX = Cur.first + dx[dir];
					int NextY = Cur.second + dy[dir];
					// 영역을 나가도 막아주고
					if(NextX < 0 || NextX >= N || NextY < 0 || NextY >= N) continue;
					// 일단 색이 다르면 영역 남기는걸 그만하고
					if(Picture[NextX][NextY] != Picture[Cur.first][Cur.second]) continue;
					// 방문한적 있어도 패스해야함
					if(RGYesVisited[NextX][NextY]) continue;

					RGYesVisited[NextX][NextY] = true;
					RGYesQ.emplace(NextX,NextY);
				}
			}
			++RedGreenYes;

		}
	}
	std::cout<<RedGreenYes;

	// 다음으로 적록색약인 사람 기준으로 보기
	int RedGreenNo = 0;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j< N; ++j)
		{
			// 방문한적 있으면 남긴거니까 다음으로 패스
			if(RGNoVisited[i][j])
			{
				continue;
			}

			RGNoQ.emplace(i,j);

			while(!RGNoQ.empty())
			{
				std::pair<int,int> Cur = RGNoQ.front(); RGNoQ.pop();
				for(int dir = 0; dir < 4; ++dir)
				{
					int NextX = Cur.first + dx[dir];
					int NextY = Cur.second + dy[dir];
					// 영역을 나가도 막아주고
					if(NextX < 0 || NextX >= N || NextY < 0 || NextY >= N) continue;
					// 만약 지금 색이 'R' 이나 'G' 면 다음 색이 'B'만아니면 됨
					char CurColor = Picture[Cur.first][Cur.second];
					if((CurColor == 'R' || CurColor=='G') && Picture[NextX][NextY] == 'B') continue;
					if((CurColor == 'B') && Picture[NextX][NextY] != 'B') continue;
					// 방문한적 있어도 패스해야함
					if(RGNoVisited[NextX][NextY]) continue;

					RGNoVisited[NextX][NextY] = true;
					RGNoQ.emplace(NextX,NextY);
				}
			}
			++RedGreenNo;

		}
	}
	std::cout<< ' ' <<RedGreenNo;


}