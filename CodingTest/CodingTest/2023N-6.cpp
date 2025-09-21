#include <vector>
#include <iostream>

int main()
{
	// 채점방식
	// 실수로 총점이 바뀔수도

	// -1000~1000
	// 선착순 제출
	// scores = [-1,-3,-2]

	// 점수는 합할 점수, 버릴점수
	// 점수를 더하다가 버리고싶은 점수는 버리기
	// 연속해서 못버림
	// 제일 높은 총점 <<

	// 총점 = 더할 수 있는 경우의수중 가장 큰 값

	// -1 -3 -2
	// 다더하면 -6 인데 -3점만 버리면 -3점
	// -3 2 4 -1 -2 -5
	
	// 비슷한 유형을 풀었는데..........
	// DP로 하면 되나?

	// 그러면
	int N;
	std::cin >> N;
	std::vector<int> Scores(N);
	for (int i =0 ; i < N; ++i)
	{
		std::cin >> Scores[i];
	}

	// DP[i][0] -> i번쨰 숫자를 안챙겼을때 여기까지의 가장 큰 값
	// DP[i][1] -> i번째 숫자를 챙겼을 때 여기까지의 가장 큰 값
	std::vector<std::vector<int>> DP(N, std::vector<int>{-1,-1});

	// -1 -3 -2
	// DP[0][0] 0		DP[1][0] 0
	// DP[0][1] -1		DP[1][1] 

	// 앞에서부터 진행해,
	DP[0][0] = 0;
	DP[0][1] = Scores[0];
	for (int i = 1; i <N; ++i)
	{
		// 연속으로 버릴 수 없으므로
		DP[i][0] = DP[i-1][1];
		DP[i][1] = std::max(DP[i-1][0] + Scores[i], DP[i-1][1] + Scores[i]);
	}

	std::cout<<std::max(DP[N-1][0], DP[N-1][1]);


}