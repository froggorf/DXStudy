#include <iostream>
#include <array>

int N,M;
std::array<int, 10> Arr;
void Solution(int CurIndex, int LastVal)
{
	if(CurIndex == M)
	{
		for(int i = 0; i < M; ++i)
		{
			std::cout << Arr[i] + 1;
			if(i == M-1)
			{
				std::cout <<'\n';
			}
			else
			{
				std::cout<<' ';
			}
		}
		return;
	}

	
	for(int i = LastVal+1; i < N; ++i)
	{
		Arr[CurIndex] = i;
		Solution(CurIndex+1, i);
	}
}

int main()
{
	std::cin >> N >> M;
	Solution(0,-1);
}