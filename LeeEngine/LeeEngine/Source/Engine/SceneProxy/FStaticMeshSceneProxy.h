// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "FPrimitiveSceneProxy.h"
#include "Engine/Material/UMaterial.h"
#include "Engine/Mesh/StaticMeshResources.h"

class UStaticMesh;

class FStaticMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	FStaticMeshSceneProxy(UINT InPrimitiveID, UINT InMeshIndex, const std::shared_ptr<UStaticMesh>& StaticMesh);
	~FStaticMeshSceneProxy() override;

	FStaticMeshRenderData* RenderData;
	
	void SetNewRenderData(const std::shared_ptr<UStaticMesh>& NewStaticMesh);

	void Draw() override;
};
