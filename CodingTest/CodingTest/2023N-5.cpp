#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
	// t와 함께 숫자카드를 제시

	// 카드를 한장씩 골라 세장을 더했을때 t보다 작거나 같아야함

	// 다만 형은 동생이 선택한거보다 큰 숫자 카드를 골라야함

	// 즉 삼형제가 숫자카드들로부터 세개를 뽑는데 이 값의 합이 t보다 작거나 같아야하고
	// 첫째 > 둘째 > 셋째 이런식으로 뽑아야함


	int T;
	std::cin>>T;
	int N;
	std::cin >> N;
	std::vector<int> D(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin >> D[i];
	}
	std::sort(D.begin(),D.end());

	// 흠... 직접 다 돌리는것 말고 좋은 방법이 있는가...
	// 일단 형이 동생보다 항상 큰수를 하래잖아..
	// 다 더했을 때 숫자카드가 10^9 까지 있으므로 다 더했을 때 INT32를 넘길 수 있음 그러므로 합은 longlong으로 관리해야함
	// n -> 10000 ..
	// 완전탐색을 하더라도 시간이 부족할만한 길이는 아닌것같음 10000개면.
	// N^3... 이긴한데.. 음.. 생각을 바꿔보자..
	// DP를 사용할 껀덕지도 안보임... 그렇다면
	// 그냥 뒷쪽부터 보면서 돌아보자 예를들어서
	// 셋째가 0, 둘째가1번 인덱스면 첫째는 N-1번부터 돌면서 처음으로 T보다 작거나 같아지는 순간 그 N개를 기록하면 됨
	int Answer = 0;

	// 0 ~ N-1
	for (int i3 = 0; i3 < N; ++i3)
	{
		// i3 ~ N-1
		for (int i2 = i3+1; i2<N; ++i2)
		{
			// N-1 ~ i2+1
			for (int i1 = N-1; i1 > i2; --i1)
			{
				// 삼형제의 숫자카드를 더해서
				long long Sum = D[i1] + D[i2] + D[i3];
				
				if (Sum <= T)
				{
					Answer += (i1 - i2);
					break;
				}
			}
		}
	}

	std::cout<<Answer;

}