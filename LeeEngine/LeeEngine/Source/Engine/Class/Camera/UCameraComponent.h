// 07.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/SceneView.h"
#include "Engine/Components/USceneComponent.h"

enum class EProjectionType
{
	Perspective,
	OrthoGraphic,
};
class UCameraComponent : public USceneComponent
{
	MY_GENERATE_BODY(UCameraComponent)
	

	UCameraComponent();
	~UCameraComponent() override = default;

	void BeginPlay() override;
	void TickComponent(float DeltaSeconds) override;

	void UpdateCameraMatrices();

	// APlayerCameraManager에서 카메라의 정보를 FScene에 업데이트 하는 함수
	void UpdateCameraData();

	const FViewMatrices& GetViewMatrices() const {return ViewMatrices;}

	float GetAspectRatio() const { return AspectRatio;}
	float GetNear() const { return Near;}
	float GetFar() const { return Far;}
	float GetFOV() const { return FOV;}
	float GetWidth() const { return Width;}
	float GetOrthoScale() const { return OrthoScale;}
	EProjectionType GetProjectionType() const { return ProjectType;}

	void SetAspectRatio(float NewAspectRatio) {AspectRatio = NewAspectRatio;}
	void SetNear(float NewNear) { Near= NewNear;}
	void SetFar(float NewFar) { Far= NewFar;}
	void SetFOV(float NewFOV) { FOV= NewFOV;}
	void SetWidth(float NewWidth) { Width= NewWidth;}
	void SetOrthoScale(float NewOrthoScale) { OrthoScale= NewOrthoScale;}
	void SetProjectionType(EProjectionType NewProjectionType) { ProjectType = NewProjectionType;};


private:
	FViewMatrices ViewMatrices;

	EProjectionType ProjectType = EProjectionType::Perspective;

	float AspectRatio = 1.777777f;
	float Near = 0.1f;         // 투영 최소거리
	float Far = 200000.0f;      // 투영 최대거리
	float FOV = XM_PI/2.0f;    // 시야각
	float Width = 1.0f;        // 직교투영 가로 범위
	float OrthoScale = 1.0f;   // 직교투영 배율
};
