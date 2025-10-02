#include <iostream>
#include <string>
#include <vector>

int Count = 0;
void Print(const std::vector<int>& Nums, int Current, std::vector<int>& Data)
{
	Data.emplace_back(Nums[Current]);
	if (Data.size() == 6)
	{
		for (int i = 0; i < 6; ++i)
		{
			std::cout<< Data[i]<<" ";
		}
		std::cout<<"\n";
		Data.pop_back();
		return;
	}


	for (int i = Current+1; i < Count; ++i)
	{
		Print(Nums, i, Data);
	}

	Data.pop_back();
}

int main()
{

	while (true)
	{
		std::cin >> Count;
		if (Count == 0) break;

		std::vector<int> Nums(Count);
		for (int i =0 ; i < Count; ++i)
		{
			std::cin >> Nums[i];
		}

		for (int i = 0; i < Count-6+1; ++i)
		{
			std::vector<int> Data;
			Print(Nums,i, Data);
		}

		std::cout<<"\n";
	}
}