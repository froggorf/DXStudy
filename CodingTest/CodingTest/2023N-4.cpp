#include <iostream>
#include <vector>

int main()
{
	// 입자묶음을 주입하면 총합이 임계치 이상으로 유지되는 한 발전기 가동

	// 임계치 미만으로 떨어지기전에 자동으로 제거하는 시스템

	// 1초에 1만큼의 에너지
	// 현재 에너지갑 max(InitialEnergy[i] - sec_time, 0)

	// 입자묶음이 발전기 속으로 들어가서
	// 에너지를 잃기 시작한 순간부터 주어지는 임계값보다 크거나 같은 상태를 최대 유지할 수 있는 발전기속 최대 체류 시간


	// 4,8,7,1,2 임계값 9

	// 시뮬레이션 돌리면 안된다는게 딱 보임.
	int N;
	std::cin>>N;
	std::vector<int> Energy(N);
	for (int i = 0; i < N; ++i)
	{
		int Val;
		std::cin>>Val;
		Energy[i] = Val;
	}
	int Threshold;
	std::cin >> Threshold;

	// 그리디 + 이분탐색으로 해볼만 할 것 같음
	int Left = 0;
	int Right = *std::max_element(Energy.begin(),Energy.end());
	int Answer = 0;
	auto Func = [](int Mid, const std::vector<int>& Energy, int Threshold)->bool
	{
		int CurVal = 0;
		for (int i = 0; i < Energy.size(); ++i)
		{
			CurVal += std::max(Energy[i] - Mid, 0);
			if (CurVal >= Threshold)
			{
				return true;
			}
		}

		return CurVal >= Threshold;
	};


	while (Left <= Right)
	{
		int Mid = (Left + Right) / 2;
		// 이 Mid 시간 만으로도 돌릴 수 있는것
		if (Func(Mid, Energy, Threshold))
		{
			Left = Mid+1;
			Answer = Mid;
		}
		else
		{
			Right = Mid-1;
		}
	}
	std::cout<<Answer;
}