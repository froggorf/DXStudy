#include "CoreMinimal.h"

#include "RenderCore/EditorScene.h"

FTransform::FTransform(XMFLOAT3 InTranslation, XMFLOAT4 InRotation, XMFLOAT3 InScale)
{
	Translation = InTranslation;
	Rotation    = InRotation;
	Scale3D     = InScale;
}

FTransform::FTransform(XMVECTOR InRotationQuat, XMFLOAT3 InTranslation, XMFLOAT3 InScale)
{
	Translation = InTranslation;
	XMStoreFloat4(&Rotation, InRotationQuat);
	Scale3D = InScale;
}

XMMATRIX FTransform::ToMatrixWithScale() const
{
	XMMATRIX OutMatrix = XMMatrixIdentity();
	OutMatrix          = XMMatrixScaling(Scale3D.x, Scale3D.y, Scale3D.z) * ToMatrixNoScale();

	return OutMatrix;
}

XMMATRIX FTransform::ToMatrixNoScale() const
{
	XMMATRIX OutMatrix = XMMatrixIdentity();

	OutMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&Rotation)) * XMMatrixTranslation(Translation.x, Translation.y, Translation.z);

	return OutMatrix;
}

FTransform FTransform::operator*(const FTransform& OtherTransform)
{
	XMMATRIX ResultMatrix = XMMatrixMultiply(OtherTransform.ToMatrixWithScale(), ToMatrixWithScale());
	XMVECTOR ResultScale, ResultRotQuat, ResultTranslate;
	XMMatrixDecompose(&ResultScale, &ResultRotQuat, &ResultTranslate, ResultMatrix);
	XMFLOAT3 RetTranslation, RetScale;
	XMStoreFloat3(&RetTranslation, ResultTranslate);

	XMStoreFloat3(&RetScale, ResultScale);

	return FTransform{ResultRotQuat, RetTranslation, RetScale};
}

XMFLOAT3 FTransform::InverseTransformPosition(const XMFLOAT3& WorldTranslation) const
{
	XMVECTOR WorldTranslationVec = XMLoadFloat3(&WorldTranslation);
	XMFLOAT3 RetVal;
	XMStoreFloat3(&RetVal, XMVector3Transform(WorldTranslationVec, XMMatrixInverse(nullptr, ToMatrixWithScale())));

	return RetVal;
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

namespace MyMath
{
	XMFLOAT4 GetRotationQuaternionToActor(const XMFLOAT3& From, const XMFLOAT3& To)
	{
		XMVECTOR Position = XMLoadFloat3(&From);
		XMVECTOR Target   = XMLoadFloat3(&To);

		XMVECTOR Direction = XMVector3Normalize(XMVectorSubtract(Target, Position));
		XMVECTOR Forward = XMVectorSet(0, 0, 1, 0);
		XMVECTOR Axis = XMVector3Cross(Forward, Direction);
		float Dot = XMVectorGetX(XMVector3Dot(Forward, Direction));
		float Angle = acosf(Dot);

		if (XMVector3Equal(Axis, XMVectorZero()))
		{
			Axis = XMVectorSet(0, 1, 0, 0);
		}
		
		XMVECTOR Quaternion = XMQuaternionRotationAxis(Axis, Angle);

		XMFLOAT4 ReturnQuaternion;
		XMStoreFloat4(&ReturnQuaternion, Quaternion);
		return ReturnQuaternion;
	}

}
