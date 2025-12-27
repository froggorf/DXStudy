#include <iostream>
#include <vector>

int main()
{
	// nlogn 으로 끊어야함
	std::cin.tie(nullptr);
	int N;
	std::cin>>N;
	std::vector<int> Nums(N);
	for (int i = 0; i <N; ++i)
	{
		std::cin>>Nums[i];
	}

	std::vector<int> Answers;
	Answers.reserve(N);
	for (int Num : Nums)
	{
		auto Lower = std::lower_bound(Answers.begin(), Answers.end(), Num);
		if (Lower == Answers.end())
		{
			Answers.emplace_back(Num);
		}
		else
		{
			*Lower = Num;
		}
	}

	std::cout<<Answers.size();
	
}