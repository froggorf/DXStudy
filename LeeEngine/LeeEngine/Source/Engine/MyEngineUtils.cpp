#include "CoreMinimal.h"
#include "MyEngineUtils.h"

#include "Animation/UAnimSequence.h"
#include "RenderCore/EditorScene.h"

FTransform FTransform::operator*(const FTransform& OtherTransform)

{
	//FTransform Result{};
	//// scale
	//DirectX::XMVECTOR ThisScale = DirectX::XMLoadFloat3(&Scale3D);
	//DirectX::XMVECTOR OtherScale = DirectX::XMLoadFloat3(&OtherTransform.Scale3D);
	//
	//DirectX::XMStoreFloat3(&Result.Scale3D, DirectX::XMVectorMultiply(ThisScale, OtherScale));
	//
	//// rot
	//DirectX::XMVECTOR ThisRotation = DirectX::XMLoadFloat4(&Rotation);
	//DirectX::XMVECTOR OtherRotation = DirectX::XMLoadFloat4(&OtherTransform.Rotation);
	//DirectX::XMVECTOR RetRotation = DirectX::XMQuaternionMultiply(OtherRotation,ThisRotation);
	////DirectX::XMVector4Normalize(RetRotation);
	//DirectX::XMStoreFloat4(&Result.Rotation, RetRotation);
	//
	//// Translation
	//DirectX::XMVECTOR ThisTranslation = DirectX::XMLoadFloat3(&Translation);
	//DirectX::XMVECTOR OtherTranslation = DirectX::XMLoadFloat3(&OtherTransform.Translation);
	//
	//DirectX::XMVECTOR ScaledOtherTranslation = DirectX::XMVectorMultiply(ThisScale, OtherTranslation);
	//DirectX::XMVECTOR RotatedOtherTranslation = DirectX::XMVector3Rotate(ScaledOtherTranslation, ThisRotation);
	//
	//DirectX::XMStoreFloat3(&Result.Translation, DirectX::XMVectorAdd(ThisTranslation, RotatedOtherTranslation));

	//return Result;

	XMMATRIX ResultMatrix = XMMatrixMultiply(OtherTransform.ToMatrixWithScale(), ToMatrixWithScale());
	XMVECTOR ResultScale, ResultRotQuat, ResultTranslate;
	XMMatrixDecompose(&ResultScale, &ResultRotQuat, &ResultTranslate, ResultMatrix);
	XMFLOAT3 RetTranslation, RetScale;
	XMStoreFloat3(&RetTranslation, ResultTranslate);

	XMStoreFloat3(&RetScale, ResultScale);

	return FTransform{ResultRotQuat, RetTranslation, RetScale};
}

XMFLOAT3 FTransform::CalculateEulerRotationFromQuaternion(const XMVECTOR& Quaternion)
{
	// TODO: 불안정한 코드이므로 사용하지 말것
	//02.19 임시 코드

	// 쿼터니언 성분 추출
	XMFLOAT4 q;
	XMStoreFloat4(&q, Quaternion);

	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	XMFLOAT3 Euler;

	// Pitch (X축 회전)
	float sinPitch = 2.0f * (w * x - y * z);
	if (std::abs(sinPitch) >= 1.0f)
	{
		// Gimbal lock: pitch가 ±90도에 가까울 때
		Euler.x = std::copysign(XM_PIDIV2, sinPitch); // ±π/2
	}
	else
	{
		Euler.x = std::asin(sinPitch);
	}

	// Yaw (Y축 회전)
	Euler.y = std::atan2(2.0f * (w * y + z * x), 1.0f - 2.0f * (x * x + y * y));

	// Roll (Z축 회전)
	Euler.z = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

	Euler.x = XMConvertToDegrees(Euler.x);
	Euler.y = XMConvertToDegrees(Euler.y);
	Euler.z = XMConvertToDegrees(Euler.z);
	return Euler;
}


XMMATRIX LinearMatrixLerp(const XMMATRIX& AMatrix, const XMMATRIX& BMatrix, float Value)
{
	XMVECTOR V1 = XMVectorLerp(AMatrix.r[0], BMatrix.r[0], Value);
	XMVECTOR V2 = XMVectorLerp(AMatrix.r[1], BMatrix.r[1], Value);
	XMVECTOR V3 = XMVectorLerp(AMatrix.r[2], BMatrix.r[2], Value);
	XMVECTOR V4 = XMVectorLerp(AMatrix.r[3], BMatrix.r[3], Value);

	return XMMATRIX{V1, V2, V3, V4};
}

XMMATRIX MyMatrixLerpForAnimation(const XMMATRIX& AMatrix, const XMMATRIX& BMatrix, float Value)
{
	XMVECTOR V1 = XMVectorLerp(AMatrix.r[0], BMatrix.r[0], Value);
	XMVECTOR V2 = XMVectorLerp(AMatrix.r[1], BMatrix.r[1], Value);
	XMVECTOR V3 = XMVectorLerp(AMatrix.r[2], BMatrix.r[2], Value);
	XMVECTOR V4 = XMVectorLerp(AMatrix.r[3], BMatrix.r[3], Value);

	XMMATRIX Out = XMMATRIX{V1, V2, V3, V4};
	return Out;
}
