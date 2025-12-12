#pragma once
#include "Engine/Class/Framework/AGameMode.h"

class ADragon;

class ATestGameMode : public AGameMode
{
	MY_GENERATE_BODY(ATestGameMode)

	void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;
	std::weak_ptr<ADragon> BossActor;
};
