#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <array>


int Row,Col;
char Board[1002][1002];
int Answer = 0;


int main()
{
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	std::cin>>Row>>Col;
	for (int x = 0 ; x < Row; ++x)
	{
		for (int y = 0 ; y < Col ; ++y)
		{
			std::cin>>Board[x][y];
		}
	}

	// 위아래 체크
	{
		// 위 체크
		{
			bool Check[1002] = {false, };
			for (int x = 0; x < Row; ++x)
			{
				for (int y = 0; y < Col - 1; ++y)
				{
					if (x == 0) Check[y] = Board[x][y] == 'X';

					if (Board[x][y] == '.' && Check[y] && Board[x][y+1]=='.' && Check[y+1])
					{
						++Answer;
						Check[y] = Check[y+1] = false;
					}

					Check[y] = Board[x][y] == 'X';
				}	
			}
		}

		// 아래 체크
		{
			bool Check[1002] = {false, };
			for (int x = Row-1; x >= 0; --x)
			{
				for (int y = 0; y < Col - 1; ++y)
				{
					if (x == Row-1) Check[y] = Board[x][y] == 'X';

					if (Board[x][y] == '.' && Check[y] && Board[x][y+1] == '.' && Check[y+1])
					{
						++Answer;
						Check[y] = Check[y+1] = false;
					}

					Check[y] = Board[x][y] == 'X';
				}	
			}
		}
		// 왼쪽 체크
		{
			bool Check[1002] = {false, };
			for (int y = 0; y < Col; ++y)
			{
				for (int x = 0; x < Row -1; ++x)
				{
					if (y == 0) Check[x] = Board[x][y] =='X';
					if (Board[x][y] == '.' && Check[x] && Board[x+1][y] == '.' && Check[x+1])
					{
						++Answer;
						Check[x] = Check[x+1] = false;
					}
					Check[x] = Board[x][y] =='X';
				}
			}
		}


		{
			bool Check[1002] = {false, };
			for (int y = Col-1; y >= 0; --y)
			{
				for (int x = 0; x < Row -1; ++x)
				{
					if (y == Col-1) Check[x] = Board[x][y] =='X';
					if (Board[x][y] == '.' && Check[x] && Board[x+1][y] == '.' && Check[x+1])
					{
						++Answer;
						Check[x] = Check[x+1] = false;
					}
					Check[x] = Board[x][y] =='X';
				}
			}
		}
	}
	std::cout<<Answer;
}