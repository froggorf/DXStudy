#include <vector>
#include <iostream>
#include <map>
#include <string>

// 문자를 하나 입력할 때 마다 기존 입력기록을 기반으로 자동완성 시켜줄 수 있는 문자열을 보여줌
// 만약 자동완성해줄 수 있는 기록이 여럿이라면 가장 최근의 것을 보여줌
// 자동완성해줄 수 있는게 없으면 바로 직전걸 보여줌

// 그리고 반환값은 입력값이 1부터 시작하므로 "inputs 의 index + 1"

struct Data
{
	std::shared_ptr<Data> Next[2];
	// 현재 자동으로 보여지는 문자열의 개수
	int LastAutoIndex = -1;
};
int main()
{
	// i 번째 문자열 -> i-1 번째까지의 문자열을 이전 입력 기록으로 간주
	int N;
	std::cin >> N;
	std::vector<std::string> Inputs(N);
	for (int i = 0; i < N; ++i)
	{
		std::cin >> Inputs[i];
	}
	// 각 문자열을 입력할 때 도우미 기능이 마지막으로 보여준 문자열의 입력 번호
	std::vector<int> Answer;

	
	std::shared_ptr<Data> Root = std::make_shared<Data>();


	for (int StrIndex = 0; StrIndex < Inputs.size(); ++StrIndex)
	{
		const std::string& Str = Inputs[StrIndex];
		std::shared_ptr<Data> R = Root;

		Answer.emplace_back(-1);
		int& CurStrAnswer = Answer[Answer.size()-1];

		bool bNowNewStr = false;
		// 이번 스트링에 대해서 데이터를 추가해줘야함
		for (int LengthIndex = 0; LengthIndex < Str.length(); ++LengthIndex)
		{
			if (!bNowNewStr)
			{
				// 일단 이전 인덱스 기록해주고
				CurStrAnswer = R->LastAutoIndex + 1;
				// 현재 인덱스가 최근이라고 남겨준다
				R->LastAutoIndex = StrIndex;		
			}

			int CurIndexVal = Str[LengthIndex] - '0';
			// 다음칸이 있다면
			if (R->Next[CurIndexVal])
			{
				R = R->Next[CurIndexVal];
			}
			// 다음 칸이 없다면, 
			else
			{
				R->Next[CurIndexVal] = std::make_shared<Data>();
				R = R->Next[CurIndexVal];
				R->LastAutoIndex = StrIndex;
				bNowNewStr = true;
			}
		}


	}

	for (int i = 0; i <Answer.size(); ++i)
	{
		std::cout << Answer[i] << " ";
	}
}

// 000 - 0 (이전 기록이 없으니까 0)
// 1110 -> 1을 눌렀는데 자동완성 될게 없으니까 "000" 이 보여짐, 그러므로 index 0 + 1 -> 1
// 01 - 0을 누르면 "000" 이 나오니까, 0 + 1 -> 1
// 001 - 00 을 누르면 "000" 이 나오니까, 0까지만 하면 "01" 과 "000" 이 나오므로 2개이상이라 안됨, 그러므로 0+1 -> 1
// 110 - 1을 치면 
