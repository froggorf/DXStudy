#pragma once
#include "Engine/Class/Framework/AGameMode.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"
#include "MyGame/Core/UMyGameInstance.h"

class UEquipmentStatusWidget;
class UStageLevelWidget;
class UDungeonDeathWidget;

class ATownGameMode : public AGameMode
{
	MY_GENERATE_BODY(ATownGameMode)

	void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;

	EEquipUpgradeResult AddEquipLevel(EEquipType Type);
	void RefreshEquipmentStatusWidget();


	std::weak_ptr<class ASanhwaCharacter> SanhwaCharacter;
	std::weak_ptr<class AGideonCharacter> GideonCharacter;

	std::shared_ptr<UEquipmentStatusWidget> EquipmentStatusWidget;
	std::shared_ptr<UStageLevelWidget> StageLevelWidget;

};

class ADungeonGameMode : public AGameMode
{
	MY_GENERATE_BODY(ADungeonGameMode)

	void Tick(float DeltaSeconds) override;
	void Register() override;
	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;

	void HandleDragonDeath();
public:
	void HandlePlayerDeath();
private:
	void ShowPlayerDeathWidget();

	float PotionCoolDownTime = 10.0f;
	bool bReturnPortalSpawned = false;
	bool bPlayerDeathWidgetRequested = false;
	std::shared_ptr<UEquipmentStatusWidget> EquipmentStatusWidget;
	std::shared_ptr<UStageLevelWidget> StageLevelWidget;
	std::shared_ptr<UDungeonDeathWidget> DeathWidget;
	FTimerHandle PlayerDeathUITimerHandle;
};

class ALoginGameMode : public AGameMode
{
	MY_GENERATE_BODY(ALoginGameMode)

	ALoginGameMode();
	void BeginPlay() override;


protected:
	std::shared_ptr<UCameraComponent> CameraComp;
};
