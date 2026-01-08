// DatabaseManager.cpp
#include "CoreMinimal.h"
#include "DatabaseManager.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

DatabaseManager& DatabaseManager::Get()
{
	static DatabaseManager Instance;
	return Instance;
}

bool DatabaseManager::Initialize(const std::string& DBPath)
{
	if (!DB.Open(DBPath))
	{
		return false;
	}

	// 테이블 객체 생성
	UserTablePtr = std::make_unique<UserTable>(DB);
	EquipmentTablePtr = std::make_unique<EquipmentTable>(DB);
	GoldTablePtr = std::make_unique<GoldTable>(DB);
	StageTablePtr = std::make_unique<StageTable>(DB);

	// 테이블 생성
	UserTablePtr->CreateTable();
	EquipmentTablePtr->CreateLevelTable();
	EquipmentTablePtr->CreateUpgradeTable();
	GoldTablePtr->CreateTable();
	StageTablePtr->CreateTable();

	// 유저 생성 시 초기화 콜백 등록
	UserTablePtr->SetOnUserCreatedCallback(
		[this](const std::string& UserID) -> bool {
			return InitializeNewUser(UserID);
		}
	);

	return true;
}

void DatabaseManager::Shutdown()
{
	UserTablePtr.reset();
	EquipmentTablePtr.reset();
	GoldTablePtr.reset();
	StageTablePtr.reset();

	DB.Close();
}

// 새 유저 초기 데이터 생성
bool DatabaseManager::InitializeNewUser(const std::string& UserID)
{
	// 트랜잭션 시작 (전체 성공 또는 전체 실패)
	if (!DB.Execute("BEGIN TRANSACTION"))
	{
		return false;
	}

	bool bSuccess = true;

	// 1. 초기 골드 설정 (예: 1000 골드)
	if (bSuccess && GoldTablePtr)
	{
		bSuccess = GoldTablePtr->SetGold(UserID, 10);
	}

	// 2. 초기 스테이지 설정 (예: 스테이지 1)
	if (bSuccess && StageTablePtr)
	{
		bSuccess = StageTablePtr->SetStage(UserID, 1);
	}

	// 3. 초기 장비 설정 (예: 기본 무기 레벨 1)
	if (bSuccess && EquipmentTablePtr)
	{
		// 장비 타입에 따라 초기화 (예시)
		bSuccess = EquipmentTablePtr->SetEquipmentLevel(UserID, EEquipType::Weapon, 1);

		if (bSuccess)
		{
			bSuccess = EquipmentTablePtr->SetEquipmentLevel(UserID, EEquipType::Glove, 1);
		}

		if (bSuccess)
		{
			bSuccess = EquipmentTablePtr->SetEquipmentLevel(UserID, EEquipType::Armor, 1);
		}

		if (bSuccess)
		{
			bSuccess = EquipmentTablePtr->SetEquipmentLevel(UserID, EEquipType::Head, 1);
		}
	}

	// 트랜잭션 완료
	if (bSuccess)
	{
		DB.Execute("COMMIT");
	}
	else
	{
		DB.Execute("ROLLBACK");
	}

	return bSuccess;
}
