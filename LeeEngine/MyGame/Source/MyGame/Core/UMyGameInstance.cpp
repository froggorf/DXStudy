#include "CoreMinimal.h"
#include "UMyGameInstance.h"

#include "MyGame/DataBase/DatabaseManager.h"

void UMyGameInstance::LoadInitialData()
{
	if (UserName.empty())
	{
		assert(false);
	}

	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize("MyGame.db"))
	{
		return;
	}

	if (auto UserGold = DBMgr.GetGoldTable()->GetGold(UserName))
	{
		Gold = *UserGold;
	}

	for (int i = 0; i < static_cast<int>(EEquipType::Count); ++i)
	{
		if (auto UserEquipData = DBMgr.GetEquipmentTable()->GetEquipmentLevel(UserName, static_cast<EEquipType>(i)))
		{
			EquipLevel[i] = *UserEquipData;
		}
	}

	if (auto StageData = DBMgr.GetStageTable()->GetStageLevel(UserName))
	{
		StageLevel = *StageData;
	}
}



bool UMyGameInstance::TryLogin(const std::string& ID, const std::string& PW)
{
	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize("MyGame.db"))
	{
		return false;
	}

	return DBMgr.GetUserTable()->ValidateLogin(ID, PW);
}

bool UMyGameInstance::TryRegister(const std::string& ID, const std::string& PW)
{
	auto& DBMgr = DatabaseManager::Get();
	if (!DBMgr.Initialize("MyGame.db"))
	{
		return false;
	}

	if (DBMgr.GetUserTable()->UserExists(ID))
	{
		return false;
	}

	return DBMgr.GetUserTable()->RegisterUser(ID, PW);
}

void UMyGameInstance::EnchantEquipLevel(EEquipType Type)
{
	++EquipLevel[static_cast<int>(Type)];

	// 데이터 쓰기
}
