#include <vector>
#include <iostream>


int main()
{
	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);

	while (true)
	{
		int Count;
		std::cin>>Count;
		if (0 == Count)
		{
			break;
		}
		std::vector<long long> Heights(Count);
		for (int i = 0; i < Count; ++i)
		{
			std::cin>>Heights[i];
		}

	}
}







//
//int main()
//{
//	// 일단 느낌상으론 n^2번을 돌리기엔
//	// 10'000'000'000
//	// 완전탐색이나 그런건 안되니까 즉 DP로 접근하는게 맞아보임
//
//	// 그럼 이건어떨까
//	// DP[x1][x2] == {i,j} 를 가지는데,
//	// x1~x2의 사각형들에 대해서 i는 높이, j는 넓이로 하는거지
//	// 그래서 x1~x2를 1칸씩 확장해가면서 값을 계산하는거로,,
//
//	std::cin.tie(nullptr);
//	std::ios::sync_with_stdio(false);
//	while (true)
//	{
//		int Count;
//		std::cin>>Count;
//		if (0 == Count)
//		{
//			break;
//		}
//
//		// DP[x1][x2] == {i,j} // x1~x2의 사각형들에 대해서 높이 i, 넓이 j...
//		// 생각해보니까 j는 (x2-x1 + 1) * i 임 따라서 그냥 높이만 구하면 됨
//		std::vector<std::vector<int>> DP(Count, std::vector<int>(Count));
//		// 초기값을 설정해준다음에
//		int Answer = 0;
//		for (int i = 0; i < Count; ++i)
//		{
//			std::cin>> DP[i][i];
//			//DP[i][i] = Squares[i];
//			Answer = std::max(Answer, DP[i][i]);
//		}
//
//		// 이제 한칸씩 확장해가면 됨
//		for (int Length = 1; Length < Count; ++Length)
//		{
//			for (int Start = 0; Start < Count - Length; ++Start)
//			{
//				DP[Start][Start + Length] = std::min(DP[Start][Start+Length-1], DP[Start+Length][Start+Length]);
//				Answer = std::max(Answer, (Length+1) * DP[Start][Start+Length]);
//			}
//		}
//		
//		std::cout<<Answer<<"\n";
//	}
//}