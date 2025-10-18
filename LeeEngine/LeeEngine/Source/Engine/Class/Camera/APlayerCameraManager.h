// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UCameraComponent.h"
#include "Engine/GameFramework/AActor.h"

enum class EViewTargetBlendFunction
{
	Linear,	// 일정한 속도
	Cubic,	// 시작과 끝에서 느리고 중간에 빠름
};

class APlayerCameraManager : public AActor
{
public:
	MY_GENERATE_BODY(APlayerCameraManager)

	APlayerCameraManager();
	~APlayerCameraManager() override;

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	std::weak_ptr<UCameraComponent> TargetCamera;

	void SetViewTargetWithBlend(const std::shared_ptr<UCameraComponent>& NewCameraComp, float BlendTime, EViewTargetBlendFunction InBlendFunction);

	void SetTargetCamera(const std::shared_ptr<UCameraComponent>& InCameraComp)
	{
		TargetCamera = InCameraComp;
	}

	FViewMatrices GetViewMatrices() const;
public:
	// 카메라 회전 최대/최소 변수 (Pitch만 쓸일이 있어서 Pitch에 대해서만 만듦)
	float ViewPitchMin = -60.0f;
	float ViewPitchMax = 60.0f;

private:
	static FTransform LastUpdateCameraTransform;
	static FViewMatrices LastUpdateViewMatrices;
	void BlendCameraAndUpdateCameraData(float DT);

	// 카메라 블렌딩 관련 변수
	float CurrentCameraBlendTime = 0.0f;
	float CameraBlendTime = 0.0f;
	bool bCameraBlending = false;
	EViewTargetBlendFunction BlendFunction;
	FTransform BlendStartTransform;
	std::shared_ptr<UCameraComponent> TargetBlendingCamera;

};