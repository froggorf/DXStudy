#include <assert.h>
#include <iostream>
#include <list>
#include <vector>


int main()
{
	// 조작은 좌우만
	// 앞으로 자동전진

	// 3개의 차선
	// 좌우 이동을 통해 장애물 피하기
	// 장애물이 있는 위치로 이동은 불가능

	// 2차선에서 시작
	// 최소한의 좌우 이동조작으로 골인에 도달해야함 <<< "최소한"
	// 1->3차선 이런게 가능한거보면 꼭 옆으로만 이동할 필요 없이 어느 라인으로던 자유롭게 이동이 가능한것으로 보임
	// 라인은 항상 3개, 2차선에서 시작
	// 그렇다면,,
	// 그리디로 접근해 볼 수 있을것같음,
	// 모든 경로상 주행가능한 길이를 놓고,
	// 그 안에서 가장 긴 도로만 계속해서 선택하면 될것같음,


	int N;
	std::cin >> N;
	// 주행 가능한 길이를 나타낸것
	std::list<int> ObstacleRoads[4];
	for (int i = 1; i < N+1; ++i)
	{
		int CurObstacleLine;
		std::cin >> CurObstacleLine;

		// i번째 위치에 장애물이 있음.
		ObstacleRoads[CurObstacleLine].emplace_back(i);
		std::cout << CurObstacleLine << " - "<<i<<"\n";
	}
	

	int MoveCount = 0;

	int CurLine = 2;
	int CurPos = 0;


	// 2차선에 장애물이 없으면 그냥 이동 안해도됨
	if (ObstacleRoads[2].empty())
	{
		std::cout<<"0";
		return 0;
	}
	
	while (true)
	{
		// 일단 CurPos 이전의 장애물 정보는 모두 지워주고,
		for (int i = 1; i < 4; ++i)
		{
			while (true)
			{
				if (ObstacleRoads[i].empty()) break;

				if (*ObstacleRoads[i].begin() < CurPos)
				{
					std::cout<<CurPos << "에서 "<< i<<"번째 장애물 제거"<<"\n";
					ObstacleRoads[i].erase(ObstacleRoads[i].begin());	
				}
				else
				{
					break;
				}
			}	    
		}

		int MaxLength = INT32_MIN;
		int MaxLengthLineIndex = -1;
		for (int i = 1; i < 4; ++i)
		{
			// i번째 차선이 장애물이 없으면 출력 후 종료
			// 무조건 여기로 들어갈 수 밖에 없으므로 무한루프 걸릴 일은 없음
			if (ObstacleRoads[i].empty())
			{				
				if (CurLine != i)
				{
					++MoveCount;	
				}
				
				std::cout<<MoveCount;
				return 0;
			}

			// 있으면 MaxLength 랑 비교해서 보기
			int CurLineObstaclePos = *ObstacleRoads[i].begin();
			// 두칸 이상 갈 수 있으면 1-> 이동 못하므로, 0-> 같은 위치에 장애물 존재
			if (CurLineObstaclePos - CurPos >= 2)
			{
				int CanGoLength = CurLineObstaclePos - CurPos;
				if (MaxLength < CanGoLength)
				{
					MaxLength = CanGoLength;
					MaxLengthLineIndex = i;
				}
			}
		}
		if (MaxLengthLineIndex == -1) assert(nullptr);

		CurPos = *ObstacleRoads[MaxLengthLineIndex].begin() - 1;
		CurLine = MaxLengthLineIndex;
		++MoveCount;
		
	}
}