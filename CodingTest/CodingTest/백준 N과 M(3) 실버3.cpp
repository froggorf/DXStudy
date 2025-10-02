#include <iostream>
#include <set>
#include <vector>


int N,M;
std::vector<int> Current;
void PrintFunc()
{
	for (int i = 1; i < N+1; ++i)
	{
		Current.emplace_back(i);
		if (Current.size() == M)
		{
			for (int j = 0; j < Current.size(); ++j)
			{
				std::cout<< Current[j] << " ";
			}
			std::cout<<"\n";
		}
		else
		{
			PrintFunc();
		}
		Current.pop_back();
	}
}

int main()
{
	// 1부터 N까지의 자연수중 M개를 고른 수열
	// 같은수를 여러번 골라도됨

	// 4, 4 면
	// 1 1 1 1
	// 1 1 1 2
	// 1 1 1 3
	// 1 1 1 4
	// 1 1 2 1
	// 1 1 2 2
	// ...
	std::cin>>N>>M;
	Current.reserve(M);

	PrintFunc();
}