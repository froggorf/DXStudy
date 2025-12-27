#include <iostream>
#include <stack>
#include <vector>

int main()
{
	int N;
	std::cin>>N;
	std::vector<int> Nums(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>>Nums[i];
	}
	std::vector<int> Answers;
	Answers.reserve(N);
	std::vector<int> Indices(N,-1);
	for (int i = 0; i < N; ++i)
	{
		int Num = Nums[i];
		auto Iter = std::lower_bound(Answers.begin(), Answers.end(), Num);
		if (Iter == Answers.end())
		{
			Answers.emplace_back(Num);
			Indices[i] = Answers.size()-1;
		}
		else
		{
			*Iter = Num;
			Indices[i] = std::distance(Answers.begin(), Iter);
		}
	}

	std::cout<<Answers.size()<<"\n";
	int Cur = Answers.size()-1;
	std::stack<int> S;
	for (int i = N-1; i >= 0; --i)
	{
		if (Indices[i] == Cur)
		{
			S.emplace(Nums[i]);
			--Cur;
		}
	}

	while (!S.empty())
	{
		std::cout<<S.top()<<" ";
		S.pop();
	}
}