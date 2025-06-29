// 06.29
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

// LineTrace의 렌더링만을 위한 용도의 클래스
// 실제로 특정 객체에 추가되기보다는 순간적인 디버그 드로우 를 위한 용도
#pragma once
#include "UShapeComponent.h"

class ULineComponent : public UShapeComponent
{
	MY_GENERATE_BODY(ULineComponent)
public:
	// Generate_Body 용으로 만든 생성자로, 실제 사용할때에는 인자가 있는 생성자를 사용해야 유효함
	ULineComponent() = default;
	ULineComponent(bool bIsHit, const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& HitLocation, const XMFLOAT3& InTraceColor = XMFLOAT3(1,0,0), const XMFLOAT3& InTraceHitColor = XMFLOAT3(0,1,0));

	void DebugDraw_RenderThread() const override;

	void CreateVertexBuffer(const XMFLOAT3& Start, const XMFLOAT3& End, const XMFLOAT3& HitLocation);
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> NonHitLineVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> HitLineVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> HitLocationBoxVertexBuffer;

	bool bIsHit = false;
	XMFLOAT3 TraceColor = XMFLOAT3(1,0,0);
	XMFLOAT3 TraceHitColor = XMFLOAT3(0,1,0);
};
