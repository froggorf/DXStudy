#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include <map>
//int main()
//{
//	// N개의 수로 이루어진 1차원 배열
//	// M개 이하의 구간 으로 나누고
//	// 점수의 최댓값을 최소로
//
//	// 구간을 m개 이하의 원소개수가 되도록 나누고,
//	// 점수는 최댓값과 최솟값의 차이.
//
//	// 모든 경우를 해서 만드는게 아니라,
//	// 투포인터로 일단 begin, begin+1 부터 해서 m까지 가면서
//	//
//
//	// 메모리가 512 니까 int 로 어.. 몇백만개는 만들 수 있을 것 같음
//	// 그러니까 모든 가능한
//	// 예를들어서 0번~3번 / 4번 ~ 7번 / 8번 ~ 10번 / ... 이런식으로 나누면
//	// [{0,3},{4,7},{8,10}, ...]  이런식으로 값을 관리하는 벡터를
//	// 여러 벡터를 만든다 치고
//	// 그 벡터들을
//	// 최댓값을 구하는 방향으로 . . .
//
//
//	int N, M;
//	std::cin.tie(nullptr);
//	std::cin>>N>>M;
//	std::vector<int> V(N);
//	for (size_t i = 0; i < V.size(); ++i)
//	{
//		std::cin >> V[i];
//	}
//
//	// 모든 경우에 대해 2칸 ~ m칸에 대한 경우를 다 더하는 건... 메모리공간에서 낭비가 너무 심한것같음..
//	// 따라서 방법을 바꿔야하는데...
//	//
//
//	//struct Data
//	//{
//	//	Data(int Max, int Min){CurMaxValue = Max; CurMinValue = Min;}
//	//	int CurMaxValue;
//	//	int CurMinValue;
//
//	//	std::vector<int>::iterator StartIter;
//	//	std::vector<int>::iterator EndIter;
//
//	//	int GetScore() const {return Score;}
//	//	void CalculateScore()
//	//	{
//	//		Score = CurMaxValue - CurMinValue;
//	//	}
//	//private:
//	//	int Score = 0;
//	//};
//	//
//	//std::vector<Data> CurData;	
//	//CurData.reserve(N/M);
//
//
//	//// 아니면!!
//	//// 최솟값이나 최댓값으로 저장하는 값이 바뀌는 경우에 한해서 값들을 만들면 되지않을까?
//
//	//int StartMaxValue = std::max(V[0], V[1]);
//	//int StartMinValue = std::min(V[0], V[1]);
//	//CurData.emplace_back(Data{StartMaxValue, StartMinValue});
//	//auto CurNextIter = V.begin()+1;
//	//while (true)
//	//{
//	//	// 일단 CurNextIter를 더하고
//	//	++CurNextIter;
//	//	// 모든 탐색이 끝났으면
//	//	if (CurNextIter == V.end())
//	//	{
//	//		// TODO: 마지막으로 감싸주고
//
//	//		break;
//	//	}
//
//	//}
//
//	// 위에 아이디어는 20분 남았을 때 폐기
//	// 1, 5, 4, 6 , 2, 1, 3, 7
//	// 값을 한개씩 일단 넣어놓은다음에
//	// 맨앞부터 해서
//	// {0, 1} 
//	// {0, 2}
//	// {0, 3}
//	// ...
//	// {0, 0 + m-1} 의 값을 가지는 구간을 일단 만들어,
//	// 그다음에
//	// {0,1} 이었으면 얘에 대해서도
//	// ... 에 대해 만들고
//	// {2, 2 + m - 1}
//	// 이런식으로 트리 구조로 전부다 만들면 되는데,
//	// 그때 이 데이터들을
//
//	// 위에 아이디어도 폐기
//	// 일단 투포인터로 접근하는데,
//	// Next는 1칸씩,
//	// Current는 Current랑 Next랑 Distance가 m 이상인 경우에만
//	// Current를 점차 나아가는 방향으로 하면 되지않을까?
//	//struct S
//	//{
//	//	int CurrentMaxVal = -1;
//	//	int CurrentMinVal = 100000;
//	//	
//	//};
//
//	//// Data[현재 CurrentIter의 Distance] = { 데이터들 , ...}
//	//std::vector<std::vector<S>> Data(N, std::vector<S>());
//	//auto CurrentIter = V.begin();
//	//auto NextIter = V.begin();
//
//	//while ( ! (CurrentIter + 1 == V.end() && NextIter + 1 == V.end()))
//	//{
//	//	// 일단 CurrentIter 을 더하는 경우를 처리하자.
//	//	auto NNIter = NextIter + 1;
//	//	if (std::distance(CurrentIter,NextIter) + 1 >= M || NNIter == V.end())
//	//	{
//	//		++CurrentIter;
//	//		
//	//		if (CurrentIter != NextIter)
//	//		{
//	//			++NextIter;
//	//			int MaxElement = -1;
//	//			int MinElement = 100000;
//	//			for (auto p = CurrentIter; CurrentIter!=NextIter; ++CurrentIter)
//	//			{
//	//				Data[std::distance(V.begin(), CurrentIter)].emplace_back()	
//	//			}
//	//			
//	//			
//	//		}
//	//	}
//
//	//	
//
//	//	++NextIter;
//	//	// 일단 이전 꺼의 결과를 꺼내,
//	//	// 이 때, 무조건 한개는 들어있어야함 (들어있게 코딩중)
//	//	int Index = std::distance(V.begin(), CurrentIter);
//	//	S& s = *Data[Index].rbegin();
//
//	//	S NewS = s;
//	//	bool bNew = false;
//	//	if (*NextIter > NewS.CurrentMaxVal)
//	//	{
//	//		bNew = true;
//	//		NewS.CurrentMaxVal = *NextIter;
//	//	}
//	//	else if (*NextIter < NewS.CurrentMinVal)
//	//	{
//	//		NewS.CurrentMinVal = *NextIter;
//	//		bNew = true;
//	//	}
//	//	if (bNew)
//	//	{
//	//		Data[Index].emplace_back(NewS);
//	//	}
//	//	
//	//}
//
//
//
//	// 이분탐색 + 그리디..
//	int MaxVal = -1;
//	int MinVal = 100002;
//	for (int i = 0; i < N; ++i)
//	{
//		if (V[i] > MaxVal)
//		{
//			MaxVal = V[i];
//		}
//		else if (V[i] < MinVal)
//		{
//			MinVal = V[i];
//		}
//	}
//	int MinScore = 0;
//	int MaxScore = MaxVal - MinVal;
//
//	
//	
//}

bool canDivide(const std::vector<int>& V, int X, int M) {
    int cnt = 1;
    int minVal = V[0], maxVal = V[0];
    for (int i = 1; i < V.size(); ++i) {
        minVal = std::min(minVal, V[i]);
        maxVal = std::max(maxVal, V[i]);
        if (maxVal - minVal > X) {
            cnt++;
            minVal = V[i];
            maxVal = V[i];
        }
    }
    return cnt <= M;
}
int N, M;

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    std::cin >> N >> M;
    std::vector<int> V(N);
    for (size_t i = 0; i < V.size(); ++i) {
        std::cin >> V[i];
    }

    int left = 0;
    int right = *std::max_element(V.begin(), V.end()) - *std::min_element(V.begin(), V.end());
    int answer = right;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (canDivide(V, mid, M)) {
            answer = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    std::cout << answer << '\n';
    return 0;
}