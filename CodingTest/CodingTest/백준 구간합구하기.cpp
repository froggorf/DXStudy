#include <iostream>
#include <vector>
std::vector<long long> Nums;
std::vector<long long> SegmentTree;

long long Init(int Node, int Start, int End)
{
	if (Start == End)
	{
		return SegmentTree[Node] = Nums[Start];
	}

	int Mid = (Start + End)/2;
	return SegmentTree[Node] = Init(Node*2+1, Start, Mid) + Init(Node*2+2, Mid+1,End);
}

void Update(int Node, int Start, int End, int Idx, long long NewValue)
{
	if (Idx < Start || Idx > End)
	{
		return;
	}

	if (Start==End)
	{
		SegmentTree[Node] = NewValue;
		return;
	}

	int Mid = (Start+End)/2;
	Update(Node*2+1, Start, Mid, Idx, NewValue);
	Update(Node*2+2, Mid+1, End, Idx, NewValue);
	SegmentTree[Node] = SegmentTree[Node*2+1] + SegmentTree[Node*2+2];
}

long long Query(int Node, int Start, int End, int Left, int Right)
{
	if (Right < Start || End < Left)
	{
		return 0;
	}

	if (Left <= Start && End <= Right)
	{
		return SegmentTree[Node];
	}

	int Mid = (Start+End) / 2;
	return Query(Node*2+1, Start, Mid, Left, Right) + Query(Node*2+2, Mid+1, End, Left, Right);
}



int main()
{
	// 수 변경이 만번, 구간합 구하는게 만번
	// 명령 자체는 2만번인데
	// 구간합 구간이 100만이니까,,
	// 구간합을 구하는 로직이 필요해보임
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	long long N,M,K;
	std::cin >> N>>M>>K;
	Nums = std::vector<long long>(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin>>Nums[i];
	}
	SegmentTree = std::vector<long long>(N*4);
	Init(0,0,N-1);

	for (int i = 0; i < M+K; ++i)
	{
		long long Command,A,B;
		std::cin>>Command>>A>>B;
		
		if (Command == 1)
		{
			--A;
			Update(0, 0,N-1, A, B);
		}
		else
		{
			--A; --B;
			std::cout<< Query(0, 0, N-1, A,B)<<"\n";
		}
	}
}