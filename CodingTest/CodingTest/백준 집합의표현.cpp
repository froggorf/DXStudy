#include <iostream>
#include <vector>

int Find(std::vector<int>& Vec, int X)
{
	if (Vec[X] == -1)
	{
		return X;
	}

	return Vec[X] = Find(Vec, Vec[X]);
}


void Union(std::vector<int>& Vec, int X, int Y)
{
	int XRoot = Find(Vec,X);
	int YRoot = Find(Vec,Y);

	// 이미 같은 집합임
	if (XRoot == YRoot)
	{
		return;
	}

	Vec[YRoot] = XRoot;
}


int main()
{

	std::cin.tie(nullptr);
	std::ios::sync_with_stdio(false);
	// 이거 유니온 파인드 쓰면 되겠는데?
	// 하나의 유니온이 하나의 집합인거지,
	int N,M;
	std::cin>>N>>M;

	// i번째는 루트가 -1이라고 적어놓고
	std::vector<int> Vec(N+1, -1);

	for (int Count = 0; Count < M; ++Count)
	{
		int Command, X, Y;
		std::cin>>Command >> X>>Y;
		if (Command == 0)
		{
			Union(Vec,X,Y);
		}
		else
		{
			int XRoot = Find(Vec,X);
			int YRoot = Find(Vec,Y);
			if (XRoot==YRoot)
			{
				std::cout<<"YES\n";
			}
			else
			{
				std::cout << "NO\n";
				
			}
		}
	}
}