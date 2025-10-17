#pragma once

#include <memory>
#include <random>


// 디버그 레벨
// DEBUG(0) - 모든 콘솔 문구 출력
// DEVELOPMENT(1) - 구현 관련 콘솔 문구 및 디버그 드로우
// SHIP(2) - 게임 출시 빌드
// 0 - DEBUG / 1 - DEVELOPMENT / 2 - SHIP
#define DEBUG_LEVEL 0

#if (DEBUG_LEVEL == 0)
#ifndef MYENGINE_BUILD_DEBUG
#define MYENGINE_BUILD_DEBUG 0
#endif
#elif (DEBUG_LEVEL == 1)
#ifndef MYENGINE_BUILD_DEVELOPMENT
		#define MYENGINE_BUILD_DEVELOPMENT 0
#endif
#elif (DEBUG_LEVEL == 2)
#ifndef MYENGINE_BUILD_SHIP
		#define MYENGINE_BUILD_SHIP
#endif
#endif

#define IS_EDITOR TRUE
#if (IS_EDITOR == TRUE)
#ifndef WITH_EDITOR
#define WITH_EDITOR
#endif
#endif

struct FPrecomputedBoneData;
extern std::shared_ptr<class UEngine> GEngine;
#ifdef WITH_EDITOR
extern std::shared_ptr<class UEditorEngine> GEditorEngine;
#endif
extern std::unique_ptr<class FDirectXDevice> GDirectXDevice;

#define Make_Transform_Dirty()\
	if(GEngine&& IsRegister())\
	{\
		std::shared_ptr<USceneComponent> Temp =shared_from_this(); \
		GEngine->MakeComponentTransformDirty(Temp);\
	}
// ============================================================================

// 언리얼 엔진에서는 템플릿으로 설정 후 double을 건네주지만 float로 고정하여 작성
struct FTransform
{
	FTransform() = default;

	FTransform(XMFLOAT3 InTranslation, XMFLOAT4 InRotation, XMFLOAT3 InScale);

	FTransform(XMVECTOR InRotationQuat, XMFLOAT3 InTranslation, XMFLOAT3 InScale);

	// 쿼터니언
	XMFLOAT4 Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	// 이동 변환 정보
	XMFLOAT3 Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 3D Scale (로컬 공간에서만 적용)
	XMFLOAT3 Scale3D = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMFLOAT3 GetTranslation() const
	{
		return Translation;
	}

	XMFLOAT4 GetRotation() const
	{
		return Rotation;
	}

	XMVECTOR GetRotationQuat() const
	{
		return XMLoadFloat4(&Rotation);
	}

	XMFLOAT3 GetScale3D() const
	{
		return Scale3D;
	}

	XMMATRIX ToMatrixWithScale() const;

	XMMATRIX ToMatrixNoScale() const;

	FTransform operator*(const FTransform& OtherTransform);
	FTransform MultiplyNoScale(const FTransform& OtherTransform);

	// 월드 좌표 -> 로컬좌표 위치
	XMFLOAT3 InverseTransformPosition(const XMFLOAT3& WorldTranslation) const;

	static XMFLOAT3 CalculateEulerRotationFromQuaternion(const XMVECTOR& Quaternion);
};

// UE Rotator.h 694 ~
inline float ClampAxis(float Angle)
{
	// (-360, 360)
	Angle = std::fmod(Angle, 360.0f);
	if (Angle < 0.0f)
	{
		// [0, 360)
		Angle += 360.0f;
	}
	return Angle;
}

inline float NormalizeAxis(float Angle)
{
	Angle = ClampAxis(Angle);
	if (Angle > 180.0f)
	{
		// shift to (-180,180]
		Angle -= 360.0f;
	}
	return Angle;
}

inline XMFLOAT3 operator+(const XMFLOAT3& A, const XMFLOAT3& B)
{
	XMFLOAT3 ReturnValue;
	XMStoreFloat3(&ReturnValue, XMVectorAdd(XMLoadFloat3(&A), XMLoadFloat3(&B)));
	return ReturnValue;
}

inline XMFLOAT2 operator+(const XMFLOAT2& A, const XMFLOAT2& B)
{
	XMFLOAT2 ReturnValue;
	XMStoreFloat2(&ReturnValue, XMVectorAdd(XMLoadFloat2(&A), XMLoadFloat2(&B)));
	return ReturnValue;
}

inline XMFLOAT4 operator+(const XMFLOAT4& A, const XMFLOAT4& B)
{
	XMFLOAT4 ReturnValue;
	XMStoreFloat4(&ReturnValue, XMVectorAdd(XMLoadFloat4(&A), XMLoadFloat4(&B)));
	return ReturnValue;
}

inline XMFLOAT3 operator/(const XMFLOAT3& A, const float Divide)
{
	assert(Divide != 0);

	XMFLOAT3 ReturnValue;
	XMStoreFloat3(&ReturnValue, XMVectorDivide(XMLoadFloat3(&A), XMVectorSet(Divide,Divide,Divide,Divide)));
	return ReturnValue;
}

inline XMFLOAT2 operator/(const XMFLOAT2& A, const float Divide)
{
	assert(Divide != 0);

	XMFLOAT2 ReturnValue;
	XMStoreFloat2(&ReturnValue, XMVectorDivide(XMLoadFloat2(&A), XMVectorSet(Divide,Divide,Divide,Divide)));
	return ReturnValue;
}

inline XMFLOAT4 operator/(const XMFLOAT4& A, const float Divide)
{
	assert(Divide != 0);

	XMFLOAT4 ReturnValue;
	XMStoreFloat4(&ReturnValue, XMVectorDivide(XMLoadFloat4(&A), XMVectorSet(Divide,Divide,Divide,Divide)));
	return ReturnValue;
}

inline XMFLOAT4 operator-(const XMFLOAT4& A, const XMFLOAT4& B)
{
	XMFLOAT4 ReturnValue;
	XMStoreFloat4(&ReturnValue, XMVectorSubtract(XMLoadFloat4(&A), XMLoadFloat4(&B)));
	return ReturnValue;
}

inline XMFLOAT3 operator-(const XMFLOAT3& A, const XMFLOAT3& B)
{
	XMFLOAT3 ReturnValue;
	XMStoreFloat3(&ReturnValue, XMVectorSubtract(XMLoadFloat3(&A), XMLoadFloat3(&B)));
	return ReturnValue;
}

inline XMFLOAT2 operator-(const XMFLOAT2& A, const XMFLOAT2& B)
{
	XMFLOAT2 ReturnValue;
	XMStoreFloat2(&ReturnValue, XMVectorSubtract(XMLoadFloat2(&A), XMLoadFloat2(&B)));
	return ReturnValue;
}



inline XMFLOAT3 operator*(const XMFLOAT3& A, const XMFLOAT3& B)
{
	XMFLOAT3 ReturnValue;
	XMStoreFloat3(&ReturnValue, XMVectorMultiply(XMLoadFloat3(&A), XMLoadFloat3(&B)));
	return ReturnValue;
}

inline XMFLOAT2 operator*(const XMFLOAT2& A, const XMFLOAT2& B)
{
	XMFLOAT2 ReturnValue;
	XMStoreFloat2(&ReturnValue, XMVectorMultiply(XMLoadFloat2(&A), XMLoadFloat2(&B)));
	return ReturnValue;
}

inline XMFLOAT4 operator*(const XMFLOAT4& A, const XMFLOAT4& B)
{
	XMFLOAT4 ReturnValue;
	XMStoreFloat4(&ReturnValue, XMVectorMultiply(XMLoadFloat4(&A), XMLoadFloat4(&B)));
	return ReturnValue;
}


inline XMFLOAT3 operator*(const XMFLOAT3& A, float Val)
{
	XMFLOAT3 ReturnValue;
	XMStoreFloat3(&ReturnValue, XMVectorScale(XMLoadFloat3(&A), Val));
	return ReturnValue;
}

inline XMFLOAT2 operator*(const XMFLOAT2& A, float Val)
{
	XMFLOAT2 ReturnValue;
	XMStoreFloat2(&ReturnValue, XMVectorScale(XMLoadFloat2(&A), Val));
	return ReturnValue;
}

inline XMFLOAT4 operator*(const XMFLOAT4& A, float Val)
{
	XMFLOAT4 ReturnValue;
	XMStoreFloat4(&ReturnValue, XMVectorScale(XMLoadFloat4(&A), Val));
	return ReturnValue;
}

inline std::wstring FloatToWString(float Value, int Pos)
{
	std::wstring Text = std::to_wstring(Value);
	return Text.substr(0, Text.find('.') + Pos + 1);
}

namespace MyMath
{
	inline XMFLOAT3 Lerp(const XMFLOAT3& A, const XMFLOAT3& B, float T)
	{
		return A*(1-T) + B*T;
	}

	inline XMFLOAT4 Lerp(const XMFLOAT4& A, const XMFLOAT4& B, float T)
	{
		return A*(1-T) + B*T;
	}

	inline XMFLOAT2 Lerp(const XMFLOAT2& A, const XMFLOAT2& B, float T)
	{
		return A*(1-T) + B*T;
	}

	inline float GetDistance(const XMFLOAT3& From, const XMFLOAT3& To)
	{
		float Distance = XMVectorGetX(XMVector3Length(XMVectorSubtract( XMLoadFloat3(&From), XMLoadFloat3(&To))));
		return Distance;
	}

	inline float GetDistance2D(const XMFLOAT2& From, const XMFLOAT2& To)
	{
		float Distance = XMVectorGetX(XMVector2Length(XMVectorSubtract(XMLoadFloat2(&From), XMLoadFloat2(&To))));
		return Distance;
	}

	inline XMVECTOR GetUnitVector(const XMVECTOR& Vector)
	{
		float Length = XMVectorGetX(XMVector3Length(Vector));
		if (Length < FLT_EPSILON)
		{
			return XMVectorZero();
		}

		return XMVectorScale(Vector, 1.0f / Length);
	}

	inline XMFLOAT3 GetDirectionUnitVector(const XMFLOAT3& From, const XMFLOAT3& To)
	{
		XMVECTOR FromVector = XMLoadFloat3(&From);
		XMVECTOR ToVector = XMLoadFloat3(&To);

		XMVECTOR DirectionVec = XMVectorSubtract(ToVector, FromVector);
		XMFLOAT3 DirectionUnitVector;
		XMStoreFloat3(&DirectionUnitVector, GetUnitVector(DirectionVec));
		return DirectionUnitVector;
	}

	XMFLOAT4 GetRotationQuaternionToActor(const XMFLOAT3& From, const XMFLOAT3& To);

	// Forward벡터 -> 액터의 Rotation 회전값
	XMFLOAT4 VectorToRotationQuaternion(const XMFLOAT3& Forward);


	static std::random_device rd;
	static std::mt19937 gen(rd());
	// 0.0f ~ 1.0f
	inline float FRand()
	{
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(gen);
	}

	// Start ~ End (int)
	inline int RandRange(int Start, int End)
	{
		std::uniform_int_distribution<int> dis(Start, End);
		return dis(gen);
	}

	// std::vector 안에 있는 값 중 임의로 반환
	inline int RandVector(const std::vector<int>& Indices)
	{
		if (Indices.empty())
		{
			return 0;
		}

		int RandomIndex = RandRange(0, static_cast<int>(Indices.size() - 1));
		return Indices[RandomIndex];
	}

	// Start ~ End (float)
	inline float FRandRange(float Start, float End)
	{
		std::uniform_real_distribution<float> dis(Start, End);
		return dis(gen);
	}

	XMFLOAT3 RotationMatrixToEulerAngle(const XMMATRIX& Mat);
	XMFLOAT3 QuaternionToEulerAngle(const XMFLOAT4& Quat);
	XMFLOAT3 QuaternionToEulerAngle(const XMVECTOR& Quat);
}
