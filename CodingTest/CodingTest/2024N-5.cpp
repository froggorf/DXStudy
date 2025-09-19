#include <iostream>
#include <map>
// programmerxxxprozmerqgram
// -> 3
// progxrammerrxproxgrammer
// -> 2
// programmerprogrammer
// -> 0
int main()
{
	// "programmer"
	// "grammproer" -> 위치가 바뀜
	// "prozmerqgram" -> 프로그래머를 섞고나서 일부 문자열을 내부에 추가

	// 주어진 문자열에서
	// 가장 오른쪽에 있는 프로그래머 문자열과 가장왼쪽에 있는 문자열 사이에 있는 인덱스의 수
	// "programmer xxx prozmerqgram -> 0~9인덱스 10글자는 프로그래머, 13~24는 programmer를 포함
	// 10 11 12 의 xxx 가 있으니까 패스

	std::string s;
	std::cin>>s;
	// 투포인터로 진행해도 될것같음
	std::map<char, int> LeftAlphaCount;
	std::map<char, int> RightAlphaCount;
	
	const std::map<char, int> ProgrammerAlphaCount = {
		{'p', 1}, {'r',3}, {'o',1},{'g',1},{'a',1},{'m',2},{'e',1}
	};

	// 앞쪽을 먼저 체크
	int BeginIdx = 0;
	int Length = static_cast<int>(s.length());
	while (BeginIdx < Length)
	{
		// 해당 char 더해주고
		LeftAlphaCount[s[BeginIdx]] += 1;
		bool Success = true;
		for (const auto& PAC : ProgrammerAlphaCount)
		{
			// 부족한게 있음
			if (LeftAlphaCount[PAC.first] < PAC.second)
			{
				Success = false;
				break;
			}
		}
		if (Success)
		{
			break;
		}

		++BeginIdx;
	}

	int RBeginIdx = static_cast<int>(Length) - 1;
	while (RBeginIdx >= 0)
	{
		RightAlphaCount[s[RBeginIdx]] +=1;
		bool Success = true;
		for (const auto& PAC : ProgrammerAlphaCount)
		{
			// 부족한게 있음
			if (RightAlphaCount[PAC.first] < PAC.second)
			{
				Success = false;
				break;
			}
		}
		if (Success)
		{
			break;
		}
		--RBeginIdx;
	}

	std::cout << RBeginIdx - BeginIdx -1;
}