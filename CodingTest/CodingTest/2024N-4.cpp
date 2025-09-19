#include <iostream>
#include <vector>
#include <queue>
int main()
{
	// 정렬 끝나기 직전의 배열
	// -> 최대 하나의 원소만 삭제하면 정렬이 완성되는것
	// 정렬이 되어있는것도 정렬끝나기 직전의 배열

	// n개의 고유한 값으 ㅣ배열
	// 몇개의 원소를 제거해야 정렬이 끝나기 직전의 배열의 상태가 되는지 <<<

	// 완전탐색으로 풀어볼만한가?
	// n이 10^5 , 100000 이므로 완전탐색은 힘들어보임
	
	// DP... 인데 DP로 풀이 생각나는게 어차피 완전탐색급인것 같아보임
	
	// BFS... 를 쓴다면?
	// 메인 노드 [3,4,2,5,1] 에서
	// 하나씩 없앤걸 자식으로 만들고
	// 거기서 자식으로 또 만들고, ...
	// 이렇게 했을때 BFS 로 하면 깊이값이 최소한의 값이 되는거니까 가능할것같음

	struct Data
	{
		std::vector<int> Arr;
		int Depth;
	};
	int N;
	std::cin>>N;
	std::vector<int> Arr(N);
	for(int i =0 ; i < N; ++i)
	{
		std::cin>>Arr[i];
	}

	// !! 무조건 오름차순인 정렬만 해당되는게 아님, 내림차순 정렬도 해당되는것임
	std::queue<Data> Q;
	Q.emplace(Data{Arr, 0});
	while(!Q.empty())
	{
		const Data& D = Q.front();
		// D의 Arr이 정렬이 끝나기 직전의 배열인지 체크
		size_t ArrSize = D.Arr.size();
		int GreaterCount = 0;
		int LessCount = 0;
		bool bIsBeforeSortArr = true;
		for (size_t i = 0; i < ArrSize - 1; ++i)
		{
			// 오름차순이면
			if (D.Arr[i] < D.Arr[i+1])
			{
				GreaterCount += 1;	
			}
			else if (D.Arr[i] > D.Arr[i+1])
			{
				LessCount += 1;
			}

			// 오름차순이랑 내림차순이 둘다 2개 이상씩 있다면
			// 정렬 직전의 배열이 아님
			if (GreaterCount >= 2 && LessCount >= 2)
			{
				bIsBeforeSortArr = false;
			}
		}

		// 만약 현재 D.Arr이 정렬 끝나기 직전의 배열이면 출력 후 종료
		if (bIsBeforeSortArr)
		{
			std::cout<< D.Depth;
			return 0;
		}

		// 아니라면 새로 제외하고서 추가해줘야함
		for (size_t RemoveIndex =0 ; RemoveIndex < ArrSize; ++RemoveIndex)
		{
			Data NewData;
			NewData.Depth = D.Depth+1;
			NewData.Arr.resize(ArrSize-1);
			int CurSize = 0;
			for (size_t Start = 0; Start < RemoveIndex; ++Start)
			{
				NewData.Arr[CurSize++] = D.Arr[Start];
			}
			for (size_t Back = RemoveIndex+1; Back<ArrSize; ++Back)
			{
				NewData.Arr[CurSize++] = D.Arr[Back];
			}
			Q.emplace(NewData);

		}


		// Data& 로 값을 받아야하므로 pop 을 뒤늦게 처리
		Q.pop();
	}


	std::cout<<-1;
	return 0;
}