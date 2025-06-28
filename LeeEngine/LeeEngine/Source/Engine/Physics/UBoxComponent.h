// 06.28
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
	MY_GENERATE_BODY(UBoxComponent)
public:
	UBoxComponent();
	physx::PxRigidActor* CreateRigidActor() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const override{return BoxMeshVertexBuffer;}

	void DebugDraw_RenderThread() const override;

	void CreateVertexBuffer();

	void SetExtent(const XMFLOAT3& NewExtent) {BoxExtent = NewExtent;}
	XMFLOAT3 GetExtent() const { return BoxExtent; }
protected:
	XMFLOAT3 BoxExtent = XMFLOAT3(0.0f,0.0f,0.0f);
	Microsoft::WRL::ComPtr<ID3D11Buffer> BoxMeshVertexBuffer;
};
