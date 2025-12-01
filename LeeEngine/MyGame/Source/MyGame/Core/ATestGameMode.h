#pragma once
#include "Engine/Class/Framework/AGameMode.h"

class ADragon;

class ATestGameMode : public AGameMode
{
	MY_GENERATE_BODY(ATestGameMode)

	void StartGame() override;
	void EndGame() override;
	std::weak_ptr<ADragon> BossActor;
};
