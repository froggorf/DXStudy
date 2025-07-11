﻿// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UPlayerInput.h"
#include "Engine/Class/Camera/APlayerCameraManager.h"
#include "Engine/GameFramework/AActor.h"

class ACharacter;

class APlayerController : public AActor
{
	MY_GENERATE_BODY(APlayerController)

	APlayerController();
	~APlayerController() = default;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	XMFLOAT4 GetControlRotation() const {return GetActorRotation();}
	void AddYawInput(float Val);
	void AddPitchInput(float Val);

	void OnPossess(ACharacter* CharacterToPossess);
	ACharacter* Character;
	std::weak_ptr<APlayerCameraManager> CameraManager;

	// 플레이어 인풋을 관리하는 오브젝트
	std::shared_ptr<UPlayerInput> PlayerInput;

};
