// 03.20
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "SceneView.h"

#include "RenderCore/EditorScene.h"

FCameraFrustum::FCameraFrustum()
{
	ProjPos[0] = XMFLOAT3(-1.f, 1.f, 0.f);
	ProjPos[1] = XMFLOAT3(1.f,  1.f, 0.f);
	ProjPos[2] = XMFLOAT3(1.f, -1.f, 0.f);
	ProjPos[3] = XMFLOAT3(-1.f,-1.f, 0.f);
				 
	ProjPos[4] = XMFLOAT3(-1.f, 1.f, 1.f);
	ProjPos[5] = XMFLOAT3(1.f,  1.f, 1.f);
	ProjPos[6] = XMFLOAT3(1.f, -1.f, 1.f);
	ProjPos[7] = XMFLOAT3(-1.f,-1.f, 1.f);
}

void FCameraFrustum::CalculateFace(const XMMATRIX& ViewInvMat, const XMMATRIX& ProjInvMat)
{
	XMMATRIX InvMat = ProjInvMat * ViewInvMat;

	// 월드공간 기준, 카메라의 시야 최대범위 위치 구하기
	XMVECTOR WorldPos[8] = {};
	for(int i = 0; i < 8; ++i)
	{
		WorldPos[i] = XMVector3TransformCoord(XMLoadFloat3(&ProjPos[i]), InvMat);
	}

	// 월드공간상의 6개의 평면 제작
	Face[static_cast<UINT>(EFrustumFace::Near)]  = XMPlaneFromPoints(WorldPos[0], WorldPos[1], WorldPos[2]); // Near
	Face[static_cast<UINT>(EFrustumFace::Far)]   = XMPlaneFromPoints(WorldPos[5], WorldPos[4], WorldPos[7]); // Far
	Face[static_cast<UINT>(EFrustumFace::Left)]  = XMPlaneFromPoints(WorldPos[4], WorldPos[0], WorldPos[7]); // Left
	Face[static_cast<UINT>(EFrustumFace::Right)] = XMPlaneFromPoints(WorldPos[1], WorldPos[5], WorldPos[6]); // Right
	Face[static_cast<UINT>(EFrustumFace::Up)]    = XMPlaneFromPoints(WorldPos[4], WorldPos[5], WorldPos[1]); // Up
	Face[static_cast<UINT>(EFrustumFace::Down)]  = XMPlaneFromPoints(WorldPos[3], WorldPos[2], WorldPos[7]); // Down
}

bool FCameraFrustum::IsSphereInside(const XMFLOAT3 WorldCenter, float SphereRadius) const
{
	for (int i = 0; i < static_cast<int>(EFrustumFace::Count); ++i)
	{
		float X = XMVectorGetX(Face[i]);
		float Y = XMVectorGetY(Face[i]);
		float Z = XMVectorGetZ(Face[i]);
		XMFLOAT3 Normal = XMFLOAT3(X, Y, Z);
		float D = XMVectorGetW(Face[i]);

		// 구가 반지름 영역까지 완전히 평면을 벗어나야 함
		// AX + BY + CZ + D > Radius
		if (XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Normal), XMLoadFloat3(&WorldCenter))) + D > SphereRadius)
		{
			return false;
		}
	}

	return true;
}

void FViewMatrices::UpdateViewMatrix(const XMFLOAT3 ViewLocation, const XMVECTOR& ViewRotation)
{
	XMVECTOR DefaultLook   = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR LookDirection = XMVector3Rotate(DefaultLook, ViewRotation);

	ViewOrigin    = ViewLocation;
	CameraRotQuat = ViewRotation;

	XMVECTOR UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&ViewOrigin), LookDirection, UpVector);
	ViewInvMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	Frustum.CalculateFace(ViewInvMatrix, ProjInvMatrix);
}

void FViewMatrices::UpdateViewMatrix(const XMMATRIX& NewViewMatrix)
{
	ViewMatrix = NewViewMatrix;
	ViewInvMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	Frustum.CalculateFace(ViewInvMatrix, ProjInvMatrix);
}

void FViewMatrices::UpdateProjectionMatrix(const XMMATRIX& NewProjectionMatrix)
{
	ProjectionMatrix = NewProjectionMatrix;
	ProjInvMatrix = XMMatrixInverse(nullptr, ProjectionMatrix);

	Frustum.CalculateFace(ViewInvMatrix, ProjInvMatrix);
}
