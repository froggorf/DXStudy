﻿// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UCameraComponent.h"
#include "Engine/GameFramework/AActor.h"

class APlayerCameraManager : public AActor
{
public:
	MY_GENERATE_BODY(APlayerCameraManager)

	APlayerCameraManager();
	~APlayerCameraManager() override;

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	std::weak_ptr<UCameraComponent> TargetCamera;

	void SetTargetCamera(const std::shared_ptr<UCameraComponent>& InCameraComp)
	{
		TargetCamera = InCameraComp;
	}

};