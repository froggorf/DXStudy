#include <iostream>
#include <vector>

int main()
{
	int N,M;
	std::cin >> N >> M;
	// DP[N][M] -> N자릿수를 만드는데 M번돌려서 만들 수 있는 경우의 수
	std::vector<std::vector<long long>> DP(N+1, std::vector<long long>(M+1,0));

	// 1자리수를 i번 돌려서 만드는 경우는 항상 1
	for (int i = 0; i <= 9; ++i)
	{
		if (i > M)
		{
			break;
		}
		DP[1][i] = 1;
	}

	for (int n = 1; n < N; ++n)
	{
		for (int m = 0; m <= M; ++m)
		{
			// DP[n][m] -> 현재 n자리수를 m번돌려서 만드는 경우가 들어있음
			for (int Rotate = 0; Rotate <= 9; ++Rotate)
			{
				// 허용하는 돌리는 횟수를 넘긴거니까 패스
				if (m + Rotate > M)
				{
					continue;
				}

				// DP[n][m] 에서 Rotate만큼 돌리면 DP[n+1][m+Rotate]를 만들 수 있음
				DP[n+1][m+Rotate] = (DP[n+1][m+Rotate] + DP[n][m]) % 1000000007;
			}	
		}
		
	}

	std::cout<< (DP[N][M]);
}

// 2 2 인 경우도 추가해줘야함,
// 그러면...
// DP[1][2] 인 경우도 추가해줘야하잖아?