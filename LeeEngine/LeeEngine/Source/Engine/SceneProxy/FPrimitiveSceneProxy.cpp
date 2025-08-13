// 03.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "FPrimitiveSceneProxy.h"

#include "Engine/RenderCore/RenderingThread.h"

void FPrimitiveSceneProxy::Draw()
{
	
	ObjConstantBuffer ocb;
	XMMATRIX          world = ComponentToWorld.ToMatrixWithScale();
	// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
	ocb.InvTransposeMatrix = (XMMatrixInverse(nullptr, world));
	ocb.World              = XMMatrixTranspose(world);
	ocb.WorldInv			= XMMatrixInverse(nullptr, world);
	ocb.MatWV				= XMMatrixMultiply(world,FRenderCommandExecutor::CurrentSceneData->GetViewMatrix());

	GDirectXDevice->MapConstantBuffer(EConstantBufferType::CBT_PerObject, &ocb, sizeof(ocb));
	
}

bool FPrimitiveSceneProxy::IsSphereInCameraFrustum(const FCameraFrustum* Frustum)
{
	float BoundSphereRadius = GetBoundSphereRadius();
	XMFLOAT3 WorldScale = ComponentToWorld.GetScale3D();
	float MaxWorldScale = max(WorldScale.x, max(WorldScale.y,WorldScale.z));
	
	return Frustum->IsSphereInside(ComponentToWorld.GetTranslation(), BoundSphereRadius*MaxWorldScale);
}

