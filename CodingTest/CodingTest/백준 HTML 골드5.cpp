#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

std::map<std::string, int> Pattern = {
	{"######...######",0},
	{"#####",1},
	{"#.####.#.####.#",2},
	{"#.#.##.#.######",3},
	{"###....#..#####",4},
	{"###.##.#.##.###",5},
	{"######.#.##.###",6},
	{"#....#....#####",7},
	{"######.#.######",8},
	{"###.##.#.######",9},
};

int main()
{
	// 0은 .
	// 1은 #

	// 시그널은 5의 배수
	// #은 검은색
	// .은 흰색
	
	int Count = 0;
	std::cin>>Count;
	std::string AllS;
	std::cin >> AllS;
	std::string S[5];
	int LastIndex = 0;
	int OneLineLength = Count/5;
	for (int i = 0 ;i < 5; ++i)
	{
		S[i] = AllS.substr(LastIndex, OneLineLength);
		LastIndex = LastIndex + OneLineLength;
	}

	std::vector<bool> bIsEmptyCol(OneLineLength, false);
	for (int i = 0; i <OneLineLength; ++i)
	{
		if (S[0][i] == '.' 
			&& S[1][i] == '.' && S[2][i] == '.' 
			&& S[3][i] == '.' && S[4][i] == '.')
		{
			bIsEmptyCol[i] = true;
		}
	}

    // 자.. 이제 저장했으니까
    for (int i = 0; i < OneLineLength;)
    {
        // 이번 열은 공백열임
		if (bIsEmptyCol[i])
		{
			++i;
			continue;
		}
		
		if (i == OneLineLength - 1 || bIsEmptyCol[i+1])
		{
			std::cout<<1;
			i+=2;
			continue;
		}

		std::string Cur = "";
		Cur.reserve(20);
		for (int j = 0; j<3; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				if (S[k][i+j] == '#')
				{
					Cur+="#";
				}
				else
				{
					Cur+=".";
				}
			}
		}

		auto p = Pattern.find(Cur);
		std::cout<<p->second;
		i+=4;

		
    }

	// 모두 .인 열이 공백
	// ###..#..
	// #.#..#..
	// ###..#..
	// #.#..#..
	// ###..#..
}