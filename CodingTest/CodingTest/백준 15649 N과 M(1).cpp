#include <iostream>
#include <vector>
using namespace std;

int N,M;

void Func(std::vector<bool> UsingNums, int CurIndex, int Depth, std::vector<int> CurData)
{
	UsingNums[CurIndex] = true;
	CurData.emplace_back(CurIndex+1);

	if(Depth == M-1)
	{
		for(int i = 0; i < CurData.size(); ++i)
		{
			std::cout<<CurData[i];
			if(i == CurData.size()-1)
			{
				std::cout<<'\n';
			}
			else
			{
				std::cout <<' ';
			}
		}
		return;
	}

	for(int i =0 ;i < N; ++i)
	{
		if(!UsingNums[i] )
		{
			Func(UsingNums, i, Depth+1, CurData);
		}
	}
}

int main()
{
	std::cin>>N>>M;
	std::vector<bool> UsingNums(N);
	std::vector<int> CurData;
	for(int i = 0; i < N; ++i)
	{
		Func(UsingNums,i, 0,CurData);	
	}
	
}