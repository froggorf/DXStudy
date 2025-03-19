// 03.20
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "SceneView.h"

void FViewMatrices::UpdateViewMatrix(const XMFLOAT3 ViewLocation, const XMVECTOR& ViewRotation)
{
	ViewOrigin = ViewLocation;
	CameraRotQuat  = ViewRotation;
	XMVECTOR DefaultLook = XMVectorSet(0.0f,0.0f,1.0f,0.0f);;
	XMVECTOR LookDirection = XMVector3Rotate(DefaultLook,ViewRotation);

	ViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&ViewOrigin), LookDirection, XMVectorSet(0.0f,1.0f,0.0f,0.0f));
}

void FViewMatrices::UpdateProjectionMatrix(const XMMATRIX NewProjectionMatrix)
{
	ProjectionMatrix = NewProjectionMatrix;

}
