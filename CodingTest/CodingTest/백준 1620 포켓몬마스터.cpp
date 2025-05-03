#include <iostream>
#include <unordered_map>
#include <vector>
std::string Pokemons[100005];
std::unordered_map<std::string, int> PokemonsNum;
int main()
{
	int PokeCount, AnsCount;
	std::cin>> PokeCount >> AnsCount;
	for(int i = 0; i < PokeCount; ++i)
	{
		std::string Name;
		std::cin>>Name;
		Pokemons[PokeCount] = Name;
		PokemonsNum[Name] = PokeCount;
	}

	std::string Get;
	for(int i = 0; i < AnsCount; ++i)
	{
		std::cin>>Get;
		if(isalpha(*Get.begin()))
		{
			std::cout<< PokemonsNum[Get];
		}
		else
		{
			std::cout<<Pokemons[atoi(Get.data())-1];
			
		}
		if(i != AnsCount-1)
		{
			std::cout<<'\n';
		}
		
	}
}