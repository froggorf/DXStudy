#include <iostream>
#include <vector>
#include <array>


int N;
int S;
std::array<int, 22> Vals;
int Answer;
int CurAddVal;

int MaxEleCount;
int CurEleCount;

void Solution(int CurIndex)
{
	
	if(MaxEleCount<= CurEleCount)
	{
		if(CurAddVal == S)
		{
			++Answer;	
		}
		
		return;
	}
	
	for(int i = CurIndex; i < N; ++i)
	{
		CurAddVal += Vals[i];
		++CurEleCount;
		Solution(i+1);
		--CurEleCount;
		CurAddVal -= Vals[i];
	}	
	
	
}


int main()
{
	std::cin.tie();
	std::cin >> N >> S;
	for(int i = 0; i < N; ++i)
	{
		std::cin>>Vals[i];
	}

	for(int i = 1; i <= N; ++i)
	{
		MaxEleCount = i;
		Solution(0);
		
	}
	

	std::cout<<Answer;
}