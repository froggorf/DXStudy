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
		ViewMatrix       = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		ProjectionMatrix = XMMatrixIdentity();
		ViewOrigin       = XMFLOAT3(0.0f, 0.0f, 0.0f);
		CameraRotQuat    = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	}

	// World To View
	XMMATRIX ViewMatrix;
	// View To Clip
	XMMATRIX ProjectionMatrix;

	// 월드 좌표에서의 카메라 위치정보
	XMFLOAT3 ViewOrigin;
	XMVECTOR CameraRotQuat;

	void UpdateViewMatrix(XMFLOAT3 ViewLocation, const XMVECTOR& ViewRotation);
	void UpdateProjectionMatrix(const XMMATRIX& NewProjectionMatrix);

	const XMMATRIX GetProjectionMatrix() const
	{
		return ProjectionMatrix;
	}

	const XMMATRIX GetViewMatrix() const
	{
		return ViewMatrix;
	}

	const XMFLOAT3 GetViewOrigin() const
	{
		return ViewOrigin;
	}

	const XMVECTOR GetCameraRotQuat() const
	{
		return CameraRotQuat;
	}
};
