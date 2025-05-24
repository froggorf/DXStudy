#include <iostream>
#include <stack>
int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::stack<int> S;

	int N;
	std::cin>>N;
	for(int i = 0; i < N; ++i)
	{
		std::string Command;
		std::cin >> Command;
		if(Command == "push")
		{
			{
				int NewNum;
				std::cin>>NewNum;
				S.emplace(NewNum);
			}
		}
		else if(Command == "pop")
		{
			{
				if(S.size()==0)
				{
					std::cout<<"-1\n";

				}else
				{
					int CurTop = S.top(); S.pop();
					std::cout<<CurTop<<'\n';
				}
			}
		}
		else if(Command == "size")
		{
			std::cout<<S.size()<<'\n';
		}
		else if(Command =="empty")
		{
			if(S.empty())
			{
				std::cout<<1<<'\n';
			}else
			{
				std::cout<<0<<'\n';
			}
		}
		else if(Command=="top")
		{
			if(S.size()==0)
			{
				std::cout<<"-1\n";

			}else
			{
				int CurTop = S.top();
				std::cout<<CurTop<<'\n';
			}
		}
		
	}
}