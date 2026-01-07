#include "CoreMinimal.h"
#include "UMyGameInstance.h"

#include "MyGame/DataBase/DatabaseManager.h"

void UMyGameInstance::LoadInitialData()
{
	if (UserName.empty())
	{
		Login();
	}


	// 골드 데이터 로드
	Gold = 1;

	// 장비 데이터 로드
	EquipLevel = {0,0,0,0};
}

void UMyGameInstance::Login()
{
	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize("game.db"))
	{
		return;
	}

	
}

void UMyGameInstance::EnchantEquipLevel(EEquipType Type)
{
	++EquipLevel[static_cast<int>(Type)];

	// 데이터 쓰기
}
