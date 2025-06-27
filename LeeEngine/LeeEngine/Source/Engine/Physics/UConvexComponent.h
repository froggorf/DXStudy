// 06.25
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UShapeComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

/*
 * StaticMesh 등의 오브젝트에 ConvexHull을 적용하여 충돌체를 관리하는 클래스
 */

class UConvexComponent : public UShapeComponent
{
	MY_GENERATE_BODY(UConvexComponent)
public:
	~UConvexComponent() override = default;

public:
	void SetStaticMesh(const std::shared_ptr<UStaticMesh>& InStaticMesh, ECollisionType Type);

	void RegisterSceneProxies() override;

	physx::PxRigidActor* CreateRigidActor() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const override{return ConvexMeshVertexBuffer;}

	void DebugDraw_RenderThread() const override;
protected:
	std::weak_ptr<UStaticMesh> BaseStaticMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConvexMeshVertexBuffer;
};
