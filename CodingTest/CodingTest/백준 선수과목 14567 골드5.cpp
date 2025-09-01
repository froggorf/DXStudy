#include <iostream>
#include <vector>
int N,M;
std::vector<std::vector<int>> Prerequisite;
std::vector<int> Answer;
void Func(int CurDepth, int CurTarget);
int main()
{
	// 선수과목을 들어야 새 과목을 들을 수 있는거같음
	// 과목수 N, 선수 조건수 M
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	std::cin>>N>>M;

	// 3 2
	// 2 3
	// 1 2
	// A B <- A 과목이 B 과목으 ㅣ선수과목
	// A 과목을 들어야 B 과목을 들을 수 있음
	// 2 -> 3
	// 1 -> 2
	// 3 들을라면 2 배워야하고
	// 2 들을라면 1 듫어야함
	// 1 -> 2 -> 3
	// 1 -> 2 -> 3


	// 1 -> 2
	// 1 -> 3
	// 2 -> 5
	// 4 -> 5
	// 1 들어야 2 3 들을 수 있고
	// 2 들어야 5 들을 수 있고
	// 4 들어야 5 들을 수 있으니까
	// 1, 4, 6 -> 2, 3 -> 5
	// 1 2 2 1 3 1


	// 그래프 문제같이 생각하면 될듯?
	// 근데 반대로,... 선수조건의 수가 복잡할 수 있으니까..
	// 해당 선수조건이 얼만큼의 레벨을 가지는지 같은걸 적어놓고
	// 하는건 어떨까?
	// 예를들어서
	// 1 -> 2 // 2 -> 3

	// 1
	// 1 -> 2
	// 1 -> 3
	// 4
	// 1 -> 2 -> 5
	// 6

	// 만약 선수과목이 여러개인 과목은 어떨까?
	// 1->2 -> 5
	// 1->3->4->5
	// std::max() 로 처리해주면 될듯..
	Prerequisite = std::vector<std::vector<int>>(N+1);
	
	for (int i = 0; i < M; ++i)
	{
		int Left, Right;
		std::cin >> Left >> Right;
		Prerequisite[Left].emplace_back(Right);
	}
	Answer = std::vector<int>(N+1,1);
	

	for (int i = 1; i < N+1; ++i)
	{
		int CurDepth = 1;
		int CurTarget = i;
		if (!Prerequisite[CurTarget].empty())
		{
			Func(CurDepth,CurTarget);	
		}
	}

	for (size_t i = 1; i < Answer.size(); ++i)
	{
		std::cout << Answer[i]<<" ";
	}
	
}

void Func(int CurDepth, int CurTarget)
{
	const std::vector<int>& Data = Prerequisite[CurTarget];

	for (const int Num : Data)
	{
		Func(CurDepth + 1, Num);
	}	
	
	Answer[CurTarget] = std::max(Answer[CurTarget], CurDepth);
}