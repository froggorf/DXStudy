#include <assert.h>
#include <iostream>
#include <vector>
int N;

/*
7
8 21 8 15 5 30 21
2
3
 */

int Process(std::vector<int>& Scores, int K, int& CurFrontRemovedVal, int& CurBackRemovedVal)
{
	int MaxVal = INT32_MIN;
	int MaxIndex = -1;
	if (N < K)
	{
		for (int i = 0; i < N; ++i)
		{
			if (Scores[i] == -1) continue;

			if (MaxVal < Scores[i])
			{
				MaxVal = Scores[i];
				MaxIndex = i;
			}
		}
	}
	else
	{
		
		// 앞쪽에 체크
		for (int i = 0; i < K + CurFrontRemovedVal; ++i)
{
			if (Scores[i] == -1) continue;

			if (MaxVal < Scores[i])
			{
				MaxVal = Scores[i];
				MaxIndex = i;
			}
		}

		// 뒤쪽에 체크
		for (int i = N - K - CurBackRemovedVal; i < N; ++i)
		{
			if (Scores[i] == -1) continue;

			if (MaxVal < Scores[i])
			{
				MaxVal = Scores[i];
				MaxIndex = i;
			}
		}
	}

	if (MaxVal == INT32_MIN) return 0;

	if (0<= MaxIndex && MaxIndex <= K+CurFrontRemovedVal)
	{
		++CurFrontRemovedVal;
	}
	else
	{
		++CurBackRemovedVal;
	}


	Scores[MaxIndex] = -1;
	return MaxVal;
	
}
int main()
{
	// 공성전 기여도 점수 저장
	// 기여도 점수 배열
	// 선택프로세스를 사용해 최고점의 기여도 점수

	// 1. 앞쪽 k개 / 뒤쪽 k개 중에 높은 점수를 선택
	// 2. 그 점수 목록에서 제거
	// 3. guilder_count 번 만큼 1~2를 반복
	// 그 이후 제거된 기여도의 총합을 구하기

	// 기여도 점수가 동일하면 왼쪽에 가까운 곳에 위치한 점수부터 선택
	// 전체 수가 k 미만인경우 전체목록 선택 가능


	std::cin>>N;
	std::vector<int> Scores(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>>Scores[i];
	}
	int GuilderCount = 0;
	std::cin>>GuilderCount;
	int k;
	std::cin>>k;

	// guilderCount 번 만큼 반복
	int Answer = 0;
	int CurFrontRemovedVal = 0;
	int CurBackRemovedVal = 0;
	for (int i = 0; i < GuilderCount; ++i)
	{
		int RemoveVal = Process(Scores, k, CurFrontRemovedVal, CurBackRemovedVal);
		Answer += RemoveVal;
	}

	std::cout<<Answer;
}