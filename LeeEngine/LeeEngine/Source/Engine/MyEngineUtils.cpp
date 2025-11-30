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

FTransform FTransform::MultiplyNoScale(const FTransform& OtherTransform)
{
	XMMATRIX ResultMatrix = XMMatrixMultiply(OtherTransform.ToMatrixNoScale(), ToMatrixNoScale());
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

void NormalizeBoneName(std::string& OriginalBoneName)
{
	if (OriginalBoneName.contains("mixamorig:"))
	{
		OriginalBoneName.replace(OriginalBoneName.begin(), OriginalBoneName.begin() + 10, "");
	}

	// 첫 글자로 빠르게 필터링
	if (OriginalBoneName.empty() || (OriginalBoneName[0] != 'W' && OriginalBoneName[0] != 'P'))
	{
		return;
	}

	if (!OriginalBoneName.contains("Wolf") &&
		!OriginalBoneName.starts_with("Pig") && 
		!OriginalBoneName.starts_with("PIG"))
	{
		return;
	}

	// 특수 문자 제거
	OriginalBoneName.erase(
		std::remove_if(OriginalBoneName.begin(), OriginalBoneName.end(), 
			[](char c) { return c == '_' || c == '-' || c == ' '; }),
		OriginalBoneName.end()
	);
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

	XMFLOAT4 ForwardVectorToRotationQuaternion(const XMFLOAT3& ForwardVector)
	{
		XMVECTOR Direction = XMVector3Normalize(XMLoadFloat3(&ForwardVector));
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

	XMFLOAT4 VectorToRotationQuaternion(const XMFLOAT3& Forward)
	{
		XMVECTOR DefaultForward = XMVectorSet(0, 0, 1, 0);
		XMVECTOR TargetForward = XMVector3Normalize(XMLoadFloat3(&Forward));

		XMVECTOR Axis = XMVector3Cross(DefaultForward, TargetForward);
		float Dot = XMVectorGetX(XMVector3Dot(DefaultForward, TargetForward));
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

	bool closeEnough(const float& a, const float& b, const float& epsilon = FLT_EPSILON)
	{
		return (epsilon > std::abs(a - b));
	}

	XMFLOAT3 RotationMatrixToEulerAngle(const XMMATRIX& Mat)
	{
		XMFLOAT4 VectorMat[4];

		XMStoreFloat4(&VectorMat[0], Mat.r[0]);
		XMStoreFloat4(&VectorMat[1], Mat.r[1]);
		XMStoreFloat4(&VectorMat[2], Mat.r[2]);
		XMStoreFloat4(&VectorMat[3], Mat.r[3]);

		XMFLOAT3 NewRot;
		if (closeEnough(VectorMat[0].z, -1.0f)) {
			float x = 0; //gimbal lock, value of x doesn't matter
			float y = XM_PI / 2;
			float z = x + atan2f(VectorMat[1].x, VectorMat[2].x);
			NewRot = XMFLOAT3{ x, y, z };
		}
		else if (closeEnough(VectorMat[0].z, 1.0f)) {
			float x = 0;
			float y = -XM_PI / 2;
			float z = -x + atan2f(-VectorMat[1].x, -VectorMat[2].x);
			NewRot = XMFLOAT3{ x, y, z };
		}
		else { //two solutions exist
			float y1 = -asinf(VectorMat[0].z);
			float y2 = XM_PI - y1;

			float x1 = atan2f(VectorMat[1].z / cosf(y1), VectorMat[2].z / cosf(y1));
			float x2 = atan2f(VectorMat[1].z / cosf(y2), VectorMat[2].z / cosf(y2));

			float z1 = atan2f(VectorMat[0].y / cosf(y1), VectorMat[0].x / cosf(y1));
			float z2 = atan2f(VectorMat[0].y / cosf(y2), VectorMat[0].x / cosf(y2));

			//choose one solution to return
			//for example the "shortest" rotation
			if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
			{
				NewRot = XMFLOAT3{ x1, y1, z1 };
			}
			else {
				NewRot = XMFLOAT3{ x2, y2, z2 };
			}
		}
		return NewRot;
	}

	XMFLOAT3 QuaternionToEulerAngle(const XMFLOAT4& Quat)
	{
		return QuaternionToEulerAngle(XMLoadFloat4(&Quat));
	}

	XMFLOAT3 QuaternionToEulerAngle(const XMVECTOR& Quat)
	{
		return RotationMatrixToEulerAngle(XMMatrixRotationQuaternion(Quat));
	}
}
