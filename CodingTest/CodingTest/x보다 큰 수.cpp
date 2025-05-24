#include <iostream>

int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);


	int N, X;
	std::cin >> N >> X;

	for(int i = 0 ; i < N; ++i)
	{
		int Num;
		std::cin>>Num;
		if(X > Num)
		{
			std::cout << Num << ' ';
		}
	}
}