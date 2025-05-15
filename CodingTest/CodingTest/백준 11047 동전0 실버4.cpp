#include <iostream>
#include <array>

std::array<int, 12> Coins;
int N, K;

int main()
{
	std::cin.tie();
	std::cin>>N>>K;
	for(int i =0 ; i< N; ++i)
	{
		std::cin>>Coins[i];
	}


	int Answer{};
	for(int i = N-1; i >= 0 ; --i)
	{
		Answer += (K / Coins[i]);
		K = K % Coins[i];

	}

	std::cout<<Answer;
}