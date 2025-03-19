// 03.20
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"

struct FViewMatrices
{
	FViewMatrices()
	{
		ViewMatrix = XMMatrixIdentity();
		ProjectionMatrix = XMMatrixIdentity();
	}
	// World To View
	XMMATRIX ViewMatrix;
	// View To Clip
	XMMATRIX ProjectionMatrix;

	// 월드 좌표에서의 카메라 위치정보
	XMFLOAT3 ViewOrigin;

	

public:
	void UpdateViewMatrix(const XMFLOAT3 ViewLocation, const XMVECTOR& ViewRotation);
	void UpdateProjectionMatrix(const XMMATRIX NewProjectionMatrix);
	inline const XMMATRIX GetProjectionMatrix() const
	{
		return ProjectionMatrix;
	}
	inline const XMMATRIX GetViewMatrix() const
	{
		return ViewMatrix;
	}
};