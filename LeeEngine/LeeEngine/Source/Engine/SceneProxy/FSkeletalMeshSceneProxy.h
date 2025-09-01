// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "FPrimitiveSceneProxy.h"
#include "Engine/Material/UMaterial.h"
#include "Engine/Mesh/USkeletalMesh.h"

class USkeletalMesh;

class FSkeletalMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	FSkeletalMeshSceneProxy(UINT PrimitiveID, UINT InMeshIndex, const std::shared_ptr<USkeletalMesh>& SkeletalMesh);
	~FSkeletalMeshSceneProxy() override;

	// 메쉬 정보
	FSkeletalMeshRenderData* RenderData;

	// 애니메이션 정보
	std::vector<XMMATRIX> BoneFinalMatrices;

	void Draw() override;

	bool IsSkeletalMesh() const override { return true; }

	float GetBoundSphereRadius() const override { return RenderData->BoundSphereRadius; }
};
