#include <algorithm>
#include <iostream>
#include <vector>

int N;
int Answer = 0;

void Func(std::vector<std::pair<int,int>>& EggData, int Cur)
{
	// 모두 돌았으면 점수 체크하고 나가기
	if (Cur == N-1)
	{
		bool bRemain = false;
		if (EggData[Cur].first > 0)
		{
			for (int i = 0; i < N-1; ++i)
			{
				if (EggData[i].first > 0)
				{
					bRemain = true;
					break;
				}
			}	
		}
		
		if (!bRemain)
		{
			int CurAnswer = static_cast<int>(std::count_if(EggData.begin(), EggData.end(),
				[](const std::pair<int,int>& A)
				{
					return A.first <= 0;
				}
			));
			Answer = std::max(Answer, CurAnswer);
			return;	
		}
		
	}

	// 이미 깨진 계란이면 다음으로 토스
	if (EggData[Cur].first <= 0)
	{
		Func(EggData, Cur+1);
		return;
	}

	bool bISShoot = false;
	for (int i = 0; i < N; ++i)
	{
		if (Cur == i) continue;
		if (EggData[i].first <= 0) continue;

		EggData[Cur].first -= EggData[i].second;
		EggData[i].first -= EggData[Cur].second;

		bISShoot = true;
		// 모두 돌았으면 점수 체크하고 나가기
		if (Cur == N-1)
		{
			int CurAnswer = static_cast<int>(std::count_if(EggData.begin(), EggData.end(),
				[](const std::pair<int,int>& A)
				{
					return A.first <= 0;
				}
			));
			Answer = std::max(Answer, CurAnswer);
		}
		else
		{
			Func(EggData, Cur+1);	
		}
		
		EggData[Cur].first += EggData[i].second;
		EggData[i].first += EggData[Cur].second;
	}

	if (!bISShoot && Cur < N-1)
	{
		Func(EggData, Cur+1);
	}
}

int main()
{
	// 계란에는 내구도와 무게
	// 내구도는 상대 무게만큼 깎임
	// 내구도가 0이되면 깨짐

	// 내구7 무게 5 vs 내구3 무게4
	// 1차 -> 내구4 무게5 vs 내구 0 무게4
	// 계란2 꺠짐

	// 계란들을 왼쪽부터 한번씩 다른 계란을 쳐 최대한 많은 계란

	// 가장왼쪽 계란을 들고
	// 

	std::cin>>N; // N->8 이니까 완전탐색 ㄱㅊ
	std::vector<std::pair<int,int>> EggData(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>>EggData[i].first >> EggData[i].second;
	}

	// 0인덱스로 1을 깰수도, 2를 꺨수도, 3을 깰수도, ...
	// 1로 0을, 1을 , 2를, ...

	Func(EggData,0);

	std::cout<<Answer;

}