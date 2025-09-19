#include <iostream>
#include <vector>

int OddNumCount = 0;
int EvenNumCount = 0;
int N = 0;
int Move(std::vector<int>& Arr)
{
	int MovedCount = 0;
	int LastEvenIndexAtOddRange = EvenNumCount;
	// 0 ~ EvenNumCount 까지 보면서
	for (int i = 0; i < EvenNumCount; ++i)
	{
		// 홀수인 수가 앞쪽에 있다면
		if (Arr[i] % 2 != 0)
		{
			// EvenNumCount ~ Arr.size 까지 보면서
			for (int j = LastEvenIndexAtOddRange; j < N; ++j)
			{
				// 짝수면
				if (Arr[j] %2 == 0)
				{
					int Swap = Arr[j];
					Arr[j] = Arr[i];
					Arr[i] = Swap;
					LastEvenIndexAtOddRange = j;
					++MovedCount;
					break;
				}
			}
		}
	}
	return MovedCount;
}

int main()
{
	std::vector<int> Arr;
	std::cin >> N;
	for (int i = 0; i < N; ++i)
	{
		int Val;
		std::cin>>Val;
		Arr.emplace_back(Val);
	}
	// 배열의 앞에는 짝수팀원이, 뒤에는 홀수팀원이
	// 필요한 최소한의 이동 수 -> Swap 을 통해 이동해야한다..
	
	for (int i = 0; i < N; ++i)
	{
		if (Arr[i]%2 ==0) ++EvenNumCount;
		else ++OddNumCount;
	}

	std::cout<< Move(Arr);
}