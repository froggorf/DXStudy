#include <algorithm>
#include <vector>
#include <iostream>
#include <queue>
#include <stack>
char Board[5][5];
int Answer = 0;
int main()
{
	for (int i =0 ; i <5; ++i)
	{
		for (int j= 0 ; j < 5; ++j)
		{
			std::cin>>Board[i][j];
		}
	}

	std::vector<int> Permutation(25,0);
	
	for (int i =0 ; i <7; ++i)
	{
		Permutation[i] = 1;
	}
	std::sort(Permutation.begin(), Permutation.end());



	do
	{
		std::vector<int> Selected;
		for (int i =0 ; i < Permutation.size(); ++i)
		{
			if (Permutation[i])
			{
				Selected.emplace_back(i);
			}
		}
		

		int SomCount = 0;
		// 일단 솜이 4개이상인지 확인하고
		for (int i =0 ; i <7; ++i)
		{
			int Cur = Selected[i];
			int x =  Cur/5;
			int y = Cur%5;
			if (Board[x][y] == 'S')
			{
				++SomCount;
				if (SomCount >=4)
				{
					break;
				}
			}
		}

		if (SomCount <4)
		{
			continue;
		}


		std::vector<bool> IsVisited(7, false);
		// 이제 이게 이어져있는지 본다
		std::queue<int> Q;
		Q.emplace(0);
		IsVisited[0] = true;

		while (!Q.empty())
		{
			int Cur = Q.front(); Q.pop();
			int CurLoc = Selected[Cur];
			// 다음 6칸에 있는 애들이 맞는 숫자가 있는지 보기
			for (int i = 0; i < 7; ++i)
			{
				if (IsVisited[i]) continue;

				int NextLoc = Selected[i];
				// 상하 인접은 그대로 OK
				if (CurLoc + 5 == NextLoc || CurLoc - 5 == NextLoc)
				{
					IsVisited[i] = true;
					Q.emplace(i);
				}
				// 좌우 인접은 같은 행일 때만 OK
				else if ((CurLoc + 1 == NextLoc && CurLoc / 5 == NextLoc / 5) ||
					(CurLoc - 1 == NextLoc && CurLoc / 5 == NextLoc / 5))
				{
					IsVisited[i] = true;
					Q.emplace(i);
				}
			}
		}
		bool No = false;
		for (int i =0 ; i < 7; ++i)
		{
			if (!IsVisited[i])
			{
				No = true;
				break;
			}
		}
		if (!No)
		{
			++Answer;
		}


	}while (std::next_permutation(Permutation.begin(), Permutation.end()));

	std::cout<<Answer;
}