#pragma once
#include "Engine/GameFramework/AActor.h"
/*
 Note
 언리얼 엔진에선 레벨 내 시스템으로 존재하고,
 플레이어 컨트롤러를 생성하고 폰을 생성하고 그런 역할들을 하지만,

 빠른 구현과 편의성을 위해 레벨 내 액터로 추가해놓는 방식으로 진행,
 유니티의 게임매니져와 같은 느낌으로 구현

 별도 레벨마다 AGameMode를 상속받아 규칙에 맞게 구현하는 방식으로 진행 예정

추가적으로, Level.myasset 내에 "GameModeClass" = AGameMode 를 통해 넣어주며,
레벨이 로드될때 함께 생성
-> InitGame 호출
-> GetWorld()->GetGameMode() 를 통해 게임모드를 받을 수 있으며
-> StartGame() / EndGame() 를 통해 구현한 게임 시스템을 시작 및 종료 가능
 */


 enum class EGameState
 {
 	EnteringMap,
	WaitingToStart,
	Pause,
	InProgress
 };

class AGameMode : public AActor
{
	MY_GENERATE_BODY(AGameMode)

	AGameMode() = default;
	~AGameMode() override = default;

	void Register() override;

	virtual void InitGame() { GameState = EGameState::WaitingToStart; }
	// BeginPlay 이후 자체적 호출을 통해 게임의 시작을 나타냄

	virtual void StartGame() {GameState = EGameState::InProgress; }
	// StartMatch 이후 EndMatch를 통해 게임 종료 + InitGame을 통해 초기화
	// 해당 클래스에서 DB를 통한 저장이 필요할경우 진행하는 등의 작업을 할 수 있을 것 같음
	virtual void EndGame()
	{
		GameState = EGameState::WaitingToStart;
		InitGame();
	};
	virtual void Pause() { GameState = EGameState::Pause; }

	bool IsGameProgressing() const {return GameState == EGameState::InProgress; }
	EGameState GetGameState() const {return GameState;}
protected:
	EGameState GameState = EGameState::EnteringMap;
};