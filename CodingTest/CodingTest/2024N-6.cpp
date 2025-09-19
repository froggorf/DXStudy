#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
	// n개의 초콜릿 이 박스에
	// weights[n]
	// 매일 하나의 초콜릿을 선택해서 절반 먹고, 절반은 다시 상자에

	// 홀수무게를 가지고 있을 경우 floor[weights[i]/2]

	// d일이 지난 후 남은 초골릿의 총 무게의 최소값

	int N;
	std::cin >> N;
	std::vector<int> Weights(N);
	for (int i = 0; i < N; ++i)
	{
		int Val;
		std::cin >> Val;
		Weights[i] = Val;
	}
	int D;
	std::cin>>D;

	// D가 100일 이하면은 그냥 시뮬레이션 돌리는게 더 빠를것같음
	//if (D < 100)
	//{
	//	int DayCount = 0;
	//	while (true)
	//	{
	//		++DayCount;
	//		auto MaxIter = std::max_element(Weights.begin(),Weights.end());
	//		*MaxIter = std::ceil(static_cast<float>(*MaxIter)/2);
	//		if (DayCount == D)
	//		{
	//			int Answer = 0;
	//			for (int Weight : Weights)
	//			{
	//				Answer += Weight;
	//			}
	//			std::cout << Answer;
	//			return 0;
	//		}
	//	}
	//	
	//}

	// D가 100보다 크면 단순히 시뮬레이션 돌려서는 최적의 결과를 낼 수 없을 것 같으므로 별도의 방법을 생각해야함
	std::sort(Weights.begin(), Weights.end(), [](int A, int B){return A > B;});
	// 60 30 6 이런식이라면						
	// 30 30 6									(1)
	// 15 30 6									(1)
	// 15 15 6									(2)
	// 8 15 6									(1)
	// 8 8 6									(2)
	// 4 8 6									(1)
	// 4 4 6									(2)
	// 4 4 3									(3) << 마지막꺼가 처음으로 삭제된 타이밍,
	//													그러면 이 때가 바로 패턴이 생기는 때,
	// 2 4 3									(1)
	// 2 2 3									(2)
	// 2 2 2									(3) << 1 -> 2 -> 3 이것이 패턴이다.
	// 그러므로, 이전까지 1-1-2-1-2-1-2-3 << 이거는 그냥 썡으로 적용시켜주고,
	//				이제 1-2-3 이라는 패턴이 생기므로 그거에 대해서는 D를 축약해줄 수 있지않을까??
	// 생각해보니까 패턴은 무조건 1->2->...->N이 될수밖에 없음..
	// 그러면 그냥 이전의 결과만을 적용시켜주면 될듯??

	int DayCount = 0;
	while (true)
	{
		DayCount += 1;
		bool bDividedLast = true;
		for (int i = 0; i < N-1; ++i)
		{
			// 앞선 인덱스 값이 더 크면,
			if (Weights[i] >= Weights[i+1])
			{
				Weights[i] = std::ceil(static_cast<float>(Weights[i])/2);
				bDividedLast = false;

				break;
			}
		}
		// 마지막에꺼를 잡을 거냐,
		if (bDividedLast)
		{
			Weights[N-1] = std::ceil(static_cast<float>(Weights[N-1])/2);
		}

		if (DayCount == D)
		{
			// 패턴을 구하던 도중 D까지 도달했음, 이게 정답이므로 그냥 더하자
			int Answer = 0;
			
			for (size_t i = 0; i < N; ++i)
			{
				Answer += Weights[i];
			}
			std::cout <<Answer;
			return 0;
		}
		else
		{
			if (bDividedLast)
			{
				break;
			}
		}
	}
	
	// 이제 초콜릿들의 무게 평탄화는 다 됐음,
	// 그러면, D-DayCount 만큼 진행을 더 해야하므로
	// (D-DayCount) / N번 만큼은 그냥 모든 값들에 대해서 std::ceil 시키면 됨
	// 그러고
	int RemainDay = D - DayCount;
	// 이 날짜들 만큼은 그냥 모든 무게들에 쳐내면 되는거고
	int RepeatTime = RemainDay/N;
	// 이 날짜들 만큼은 앞에서부터 직접 다 시뮬레이션 해줘야함
	int SimulationTime = RemainDay - (N*RepeatTime);

	// RepeatTime 만큼 다 나눠주고
	int DivideVal = std::pow(2, RepeatTime);
	int Answer = 0;
	for (int i = 0; i < N; ++i)
	{
		if (i < SimulationTime)
		{
			Weights[i] = std::ceil(static_cast<float>(Weights[i]) / (DivideVal*2));	
		}
		else
		{
			Weights[i] = std::ceil(static_cast<float>(Weights[i]) / DivideVal);	
		}
		std::cout <<Weights[i] << " ";
		Answer += Weights[i];
	}

	std::cout<<Answer;
	return 0;
}