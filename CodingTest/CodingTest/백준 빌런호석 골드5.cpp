#include <iostream>
#include <string>
#include <vector>

std::vector<std::vector<int>> ReverseMap = {
	// 0
	{0, 4, 3, 3, 4, 3, 2, 3, 1, 2},
	// 1
	{4, 0, 5, 3, 2, 5, 6, 1, 5, 4},
	// 2
	{3, 5, 0, 2, 5, 4, 3, 4, 2, 3},
	// 3
	{3, 3, 2, 0, 3, 2, 3, 2, 2, 1},
	// 4
	{4, 2, 5, 3, 0, 3, 4, 3, 3, 2},
	// 5
	{3, 5, 4, 2, 3, 0, 1, 4, 2, 1},
	// 6
	{2, 6, 3, 3, 4, 1, 0, 5, 1, 2},
	// 7
	{3, 1, 4, 2, 3, 4, 5, 0, 4, 3},
	// 8
	{1, 5, 2, 2, 3, 2, 1, 4, 0, 1},
	// 9
	{2, 4, 3, 1, 2, 1, 2, 3, 1, 0},
};



int N,K,P,X;
int Answer = 0;
std::string InitFloor;
void BackTrack(std::string& CurFloor, int CurIndex, int CurCount)
{
	

	if (CurIndex >= K)
	{
		if (InitFloor == CurFloor)
		{
			return;
		}
		if (CurCount <= P)
		{
			
			// N층을 넘어서는 데이터는 없애야함
			int CurFloorInt = std::stoi(CurFloor);
			if (CurFloorInt <= 0 || CurFloorInt > N)
			{
				return;
			}
			++Answer;
		}
		return;
	}

	char Last = CurFloor[CurIndex];
	int LastInt = Last - '0';
	for (int i = 0; i <= 9; ++i)
	{
		char CurChar = static_cast<char>(static_cast<int>('0') + i);
		CurFloor[CurIndex] = CurChar;
		BackTrack(CurFloor, CurIndex + 1, CurCount + ReverseMap[LastInt][i]);
		CurFloor[CurIndex] = Last;
	}
	
}

int main(){
	// 일단
	// 각 숫자들이 다른숫자로 반전하는데 필요한 개수를 구해놓고
	// P개에 대해서 반전이 가능한 애들을 모아놓는다
	

	// N -> N층
	// K -> K자리수가 보임
	// P -> 최대 반전 개수
	// X -> 현재 층
	std::cin >> N >> K >> P >> X;
	std::string CurFloor = std::to_string(X);
	CurFloor.reserve(K);
	while (static_cast<int>(CurFloor.length()) < K)
	{
		CurFloor = "0" + CurFloor;
	}
	InitFloor = CurFloor;
	BackTrack(CurFloor, 0, 0);
	std::cout<<Answer;
}