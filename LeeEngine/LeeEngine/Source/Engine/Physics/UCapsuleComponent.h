// 06.29
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UShapeComponent.h"

class UCapsuleComponent : public UShapeComponent
{
	MY_GENERATE_BODY(UCapsuleComponent)
public:
	UCapsuleComponent();
	physx::PxRigidActor* CreateRigidActor() override;

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const override{return CapsuleMeshVertexBuffer;}

	void DebugDraw_RenderThread() const override;

	void CreateVertexBuffer();

	float GetRadius() const { return Radius; }
	void SetRadius(float NewRadius) {Radius = NewRadius;}
	float GetHalfHeight() const {return HalfHeight;}
	void SetHalfHeight(float NewHalfHeight) {HalfHeight = NewHalfHeight;}

protected:
	float Radius = 0.0f;
	float HalfHeight = 0.0f;
	Microsoft::WRL::ComPtr<ID3D11Buffer> CapsuleMeshVertexBuffer;
	UINT CapsuleMeshVertexCount;
	
};
