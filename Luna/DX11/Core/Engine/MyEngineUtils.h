#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>

#include <iostream>

#include "d3dUtil.h"

// 언리얼 엔진에서는 템플릿으로 설정 후 double을 건네주지만 float로 고정하여 작성
struct FTransform
{
	FTransform() = default;
	FTransform(DirectX::XMFLOAT3 InTranslation, DirectX::XMFLOAT4 InRotation, DirectX::XMFLOAT3 InScale)
	{
		Translation = InTranslation;
		Rotation = InRotation;
		Scale3D = InScale;
	}
	FTransform(DirectX::XMVECTOR InRotationQuat, DirectX::XMFLOAT3 InTranslation, DirectX::XMFLOAT3 InScale)
	{
		Translation = InTranslation;
		DirectX::XMStoreFloat4(&Rotation, InRotationQuat);
		Scale3D = InScale;
	}
	
	// 쿼터니언
	DirectX::XMFLOAT4 Rotation = DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f);

	// 이동 변환 정보
	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);

	// 3D Scale (로컬 공간에서만 적용)
	DirectX::XMFLOAT3 Scale3D = DirectX::XMFLOAT3(1.0f,1.0f,1.0f);

	const DirectX::XMFLOAT3& GetLocation() const { return Translation; }
	const DirectX::XMFLOAT4& GetRotationQuat() const { return Rotation; }
	const DirectX::XMFLOAT3& GetScale3D() const { return Scale3D; }

	DirectX::XMMATRIX ToMatrixWithScale() const
	{
		DirectX::XMMATRIX OutMatrix = DirectX::XMMatrixIdentity();
		OutMatrix = DirectX::XMMatrixScaling(Scale3D.x,Scale3D.y,Scale3D.z) * ToMatrixNoScale();

		return OutMatrix;
	}

	DirectX::XMMATRIX ToMatrixNoScale() const
	{
		DirectX::XMMATRIX OutMatrix = DirectX::XMMatrixIdentity();
		
		OutMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&Rotation))
					* DirectX::XMMatrixTranslation(Translation.x,Translation.y,Translation.z);

		return OutMatrix;
	}

	FTransform operator*(const FTransform& OtherTransform)
	{
		FTransform Result{};

		// scale
		DirectX::XMVECTOR ThisScale = DirectX::XMLoadFloat3(&Scale3D);
		DirectX::XMVECTOR OtherScale = DirectX::XMLoadFloat3(&OtherTransform.Scale3D);

		DirectX::XMStoreFloat3(&Result.Scale3D, DirectX::XMVectorMultiply(ThisScale, OtherScale));

		// rot
		DirectX::XMVECTOR ThisRotation = DirectX::XMLoadFloat4(&Rotation);
		DirectX::XMVECTOR OtherRotation = DirectX::XMLoadFloat4(&OtherTransform.Rotation);
		DirectX::XMVECTOR RetRotation = DirectX::XMQuaternionMultiply(ThisRotation,OtherRotation);
		//DirectX::XMVector4Normalize(RetRotation);
		DirectX::XMStoreFloat4(&Result.Rotation, RetRotation);

		// Translation
		DirectX::XMVECTOR ThisTranslation = DirectX::XMLoadFloat3(&Translation);
		DirectX::XMVECTOR OtherTranslation = DirectX::XMLoadFloat3(&OtherTransform.Translation);

		DirectX::XMVECTOR ScaledOtherTranslation = DirectX::XMVectorMultiply(ThisScale, OtherTranslation);
		DirectX::XMVECTOR RotatedOtherTranslation = DirectX::XMVector3Rotate(ScaledOtherTranslation, ThisRotation);

		DirectX::XMStoreFloat3(&Result.Translation, DirectX::XMVectorAdd(ThisTranslation, RotatedOtherTranslation));

		return Result;
	}

	XMFLOAT3 GetEulerRotation()
	{
		//02.19 임시 코드
		// 쿼터니언 성분 추출
		XMVECTOR RotationQuat = XMLoadFloat4(&Rotation);
		float x = XMVectorGetX(RotationQuat);
		float y = XMVectorGetY(RotationQuat);
		float z = XMVectorGetZ(RotationQuat);
		float w = XMVectorGetW(RotationQuat);

		// 오일러 각 계산
		XMFLOAT3 euler;

		// Pitch (X축 회전)
		float sinPitch = 2.0f * (w * y - z * x);
		if (std::abs(sinPitch) >= 1.0f) {
			// Gimbal Lock: Pitch가 ±90도일 때
			euler.x = std::copysign(XM_PI / 2, sinPitch); // ±90도 (라디안)
		} else {
			euler.x = std::asin(sinPitch); // 라디안
		}

		// Yaw (Y축 회전)
		euler.y = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

		// Roll (Z축 회전)
		euler.z = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

		// 라디안 -> 도(degree) 변환
		euler.x *= (180.0f / XM_PI);
		euler.y *= (180.0f / XM_PI);
		euler.z *= (180.0f / XM_PI);

		return euler;
	}
};



extern std::unique_ptr<class UEngine> GEngine;