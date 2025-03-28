// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "FPrimitiveSceneProxy.h"

class USkeletalMesh;

class FSkeletalMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	FSkeletalMeshSceneProxy(UINT PrimitiveID, const std::shared_ptr<USkeletalMesh>& SkeletalMesh);
	virtual ~FSkeletalMeshSceneProxy() override;

	std::shared_ptr<USkeletalMesh> SkeletalMesh;

	std::vector<DirectX::XMMATRIX> BoneFinalMatrices;

	virtual void Draw() override;
};
