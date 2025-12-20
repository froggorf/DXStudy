#pragma once
#include "Engine/Class/Framework/AGameMode.h"

class ATownGameMode : public AGameMode
{
	MY_GENERATE_BODY(ATownGameMode)

	void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
	void StartGame() override;
	void EndGame() override;
};
