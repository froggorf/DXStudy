#include <iostream>
#include <array>
#include <vector>
std::array<std::array<int,17>,17> Arr;
int N;
std::vector<std::pair<int,int>> CurQueenPos;
int Answer = 0;


void Solution(int CurCount)
{
	// 만들어 진 상태
	if(CurCount == N)
	{
		++Answer;
		return;
	}

	for(int i = 0 ; i < N; ++i)
	{
		std::pair<int, int> CurPos = {CurCount, i};
		bool bCanQueenPos = true;
		for(int j = 0 ; j < CurQueenPos.size(); ++j)
		{
			// 대각선을 체크
			if(std::abs(CurPos.first - CurQueenPos[j].first) == std::abs(CurPos.second - CurQueenPos[j].second))
			{
				bCanQueenPos = false;
				break;
			}

			// 좌우는 체크할 필요가 없음
			// 상하 체크
			if(CurPos.second == CurQueenPos[j].second)
			{
				bCanQueenPos= false;
				break;
			}
		}

		// 가능하다면 배치하고 넘어가기
		if(bCanQueenPos)
		{
			CurQueenPos.emplace_back(CurPos);
			Solution(CurCount+1);
			CurQueenPos.pop_back();
		}
	}
}


int main()
{
	std::cin>> N;
	Solution(0);
	std::cout<<Answer;
}