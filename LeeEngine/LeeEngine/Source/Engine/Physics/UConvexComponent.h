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
	UConvexComponent();
	~UConvexComponent() override = default;

public:
	void SetStaticMesh(const std::shared_ptr<UStaticMesh>& InStaticMesh);
	std::shared_ptr<UStaticMesh> GetStaticMesh() const;

	void RegisterSceneProxies() override;

	physx::PxRigidActor* CreateRigidActor() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const override{return ConvexMeshVertexBuffer;}

	void DebugDraw_RenderThread() const override;

	// NavMesh 를 렌더링하기 위한 목적으로 임시적으로 VertexBuffer를 만들어 관리하는 함수
protected:
	std::weak_ptr<UStaticMesh> BaseStaticMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConvexMeshVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> TriangleMeshVertexBuffer;

	bool bRenderNavMesh = false;
};
