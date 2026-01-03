#pragma once
#include "Engine/Class/Framework/AGameMode.h"
#include "MyGame/Character/Player/AMyGameCharacterBase.h"

class ATownGameMode : public AGameMode
{
	MY_GENERATE_BODY(ATownGameMode)

	void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;

	void AddEquipLevel(EEquipType Type);


	std::weak_ptr<class ASanhwaCharacter> SanhwaCharacter;
	std::weak_ptr<class AGideonCharacter> GideonCharacter;
	static std::array<int, static_cast<int>(EEquipType::Count)> EquipLevel;

};

class ADungeonGameMode : public AGameMode
{
	MY_GENERATE_BODY(ADungeonGameMode)

	void Tick(float DeltaSeconds) override;
	void Register() override;
	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;
};