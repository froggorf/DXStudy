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
	// 쿼터니언
	DirectX::XMFLOAT4 Rotation = DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f);

	// 이동 변환 정보
	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);

	// 3D Scale (로컬 공간에서만 적용)
	DirectX::XMFLOAT3 Scale3D = DirectX::XMFLOAT3(1.0f,1.0f,1.0f);

	const DirectX::XMFLOAT3& GetLocation() const { return Translation; }
	const DirectX::XMFLOAT4& GetRotationQuat() const { return Rotation; }
	const DirectX::XMFLOAT3& GetScale3D() const { return Scale3D; }
	
};



extern class UEngine* GEngine;