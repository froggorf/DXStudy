#include <iostream>
#include <vector>

int N, M;
std::vector<int> Nums;



bool Check(int Mid)
{
	int CurCount = 1;
	int MaxVal = Nums[0];
	int MinVal = Nums[0];
	for (int i = 1; i < N; ++i)
	{
		MaxVal = std::max(MaxVal, Nums[i]);
		MinVal = std::min(MinVal, Nums[i]);
		if (MaxVal - MinVal > Mid)
		{
			++CurCount;
			MaxVal = Nums[i];
			MinVal = Nums[i];
		}
	}

	return CurCount > M;	
}

int main()
{
	// 배열을 M개 이하로 나눠야함
	// 그 때, 구간의 최댓값을 최소로

	// 구간의 최대 - 최소 값으로 가능한 것은
	// 모두 같은수의 0 ~ (가장 큰 수 - 가장 작은 수) 이므로
	// 0 ~ (가장큰수 - 가장 작은수) 내에서 조건을 만족하도록 하여
	// 이분탐색을 통해 찾으면 됨
	std::cin.tie(nullptr);

	std::cin>>N>>M;
	Nums.resize(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin >> Nums[i];	
	}
	

	int Left = 0;

	int MaxValue = 0xffff'ffff;
	int MinValue = 0x0fff'ffff;
	// n번만에 돌기 위해 max_element / min_element 말고 해당 로직 사용
	for (int i = 0; i < N; ++i)
	{
		MaxValue = std::max(MaxValue, Nums[i]);
		MinValue = std::min(MinValue, Nums[i]);
	}
	int Right = MaxValue - MinValue;
	int Answer = Right;

	// 구간이 M개 이하면 합격인거니까
	// 앞에서부터 최대, 최솟값을 뺐을 때 Mid 보다 같거나 작은지 체크하고
	// Mid 보다 커지는 순간 새로 구간을 나눠서 구간을 추가해주고
	// 그 카운트보다 늘어나면, 해당 부분은 안되는거니까 오른쪽으로 넘겨주기 위해서
	// Left = Mid+1 해줘야함
	
	while (Left <= Right)
	{
		int Mid = (Left + Right) / 2;
		if (Check(Mid))
		{
			Left = Mid +1;
		}
		else
		{
			Answer = Mid;
			Right = Mid-1;
		}
	}

	std::cout << Answer;
}