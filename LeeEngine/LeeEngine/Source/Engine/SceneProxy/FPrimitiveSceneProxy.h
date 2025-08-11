// 03.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/Material/UMaterial.h"

// 언리얼엔진에서는 Func_GameThread(); Func_RenderThread(); 방식으로 쓰레드를 나누어 작업

// LeeEngine 내에선 FPrimitiveComponent - FPrimitiveSceneProxy가 1:1로 연결되며
// SceneRenderer 내 unordered_map { PrimitiveComponentID (UINT), PrimitiveSceneProxy } 로 관리
// 게임 쓰레드에서는 PrimitiveComponentID 만으로 접근하며,
// 렌더 쓰레드에서는 ID를 받아 SceneProxy를 직접적으로 조정하여
// 상호배제를 진행

struct FCameraFrustum;

class FPrimitiveSceneProxy
{
public:
	FPrimitiveSceneProxy(UINT InPrimitiveID)
		: PrimitiveID(InPrimitiveID)
	{
	}

	virtual      ~FPrimitiveSceneProxy() = default;
	virtual void Draw();

	UINT GetPrimitiveID() const
	{
		return PrimitiveID;
	}

	// 복사되도록 설정
	void SetSceneProxyWorldTransform(FTransform NewComponentToWorld)
	{
		ComponentToWorld = NewComponentToWorld;
	}

	UINT GetMeshIndex() const
	{
		return MeshIndex;
	}

	UINT GetMaterialID() const
	{
		return MaterialInterface->GetMaterialID();
	}

	std::shared_ptr<UMaterialInterface> GetMaterialInterface() const
	{
		return MaterialInterface;
	}

	bool IsSphereInCameraFrustum(const FCameraFrustum* Frustum);
	// TODO: 08.11 기준 StaticMesh / SkeletalMesh에 대해서만 바운드 스피어를 설정하였기에 다른 씬 프록시가 잘 그려질 수 있도록 값을 간략히만 조정
	virtual float GetBoundSphereRadius() const {return FLT_MAX/10000;}

protected:
	UINT PrimitiveID;
	UINT MeshIndex;
	// 머테리얼 정보
	std::shared_ptr<UMaterialInterface> MaterialInterface;

	FTransform ComponentToWorld;
};
