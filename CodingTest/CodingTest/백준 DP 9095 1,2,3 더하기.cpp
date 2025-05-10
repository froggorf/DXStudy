#include <iostream>
#include <array>

//1을 만들기 위해선 1 -> 초기값
// 2를 만들기 위해선 2 -> 초기값
// 3을 만들기 위해선 2 -> 초기값
// 4부터 점화식을 세워보자

std::array<int, 15> Nums;

int N;

int main()
{
	std::cin.tie();
	std::cin>>N;

	Nums[1] = 1;
	Nums[2] = 2;
	Nums[3] = 4;
	for(int i = 4; i < 12; ++i)
	{
		Nums[i] = 0;
		// 4부터니까..
		// 1을 더한경우
		Nums[i] += Nums[i-1];
		// 2를 더한경우
		Nums[i] += Nums[i-2];
		// 3을 더한경우
		Nums[i] += Nums[i-3];
	}


	for(int i = 0; i < N; ++i)
	{
		int Val;
		std::cin>>Val;
		std::cout<<Nums[Val];
		if(i!=N)
		{
			std::cout<<'\n';
		}
	}
}