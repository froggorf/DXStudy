#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>

#include <iostream>

#include "DirectX/d3dUtil.h"

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

	FTransform(XMFLOAT3 InTranslation, XMFLOAT4 InRotation, XMFLOAT3 InScale)
	{
		Translation = InTranslation;
		Rotation    = InRotation;
		Scale3D     = InScale;
	}

	FTransform(XMVECTOR InRotationQuat, XMFLOAT3 InTranslation, XMFLOAT3 InScale)
	{
		Translation = InTranslation;
		XMStoreFloat4(&Rotation, InRotationQuat);
		Scale3D = InScale;
	}

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

	XMMATRIX ToMatrixWithScale() const
	{
		XMMATRIX OutMatrix = XMMatrixIdentity();
		OutMatrix          = XMMatrixScaling(Scale3D.x, Scale3D.y, Scale3D.z) * ToMatrixNoScale();

		return OutMatrix;
	}

	XMMATRIX ToMatrixNoScale() const
	{
		XMMATRIX OutMatrix = XMMatrixIdentity();

		OutMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&Rotation)) * XMMatrixTranslation(Translation.x, Translation.y, Translation.z);

		return OutMatrix;
	}

	FTransform operator*(const FTransform& OtherTransform);

	// 월드 좌표 -> 로컬좌표 위치
	XMFLOAT3 InverseTransformPosition(const XMFLOAT3& WorldTranslation) const
	{
		XMVECTOR WorldTranslationVec = XMLoadFloat3(&WorldTranslation);
		XMFLOAT3 RetVal;
		XMStoreFloat3(&RetVal, XMVector3Transform(WorldTranslationVec, XMMatrixInverse(nullptr, ToMatrixWithScale())));

		return RetVal;
	}

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

XMMATRIX MyMatrixLerpForAnimation(const XMMATRIX& AMatrix, const XMMATRIX& BMatrix, float Value);