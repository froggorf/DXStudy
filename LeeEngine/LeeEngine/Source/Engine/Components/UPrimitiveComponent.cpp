// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UPrimitiveComponent.h"

#include "Engine/RenderCore/RenderingThread.h"

UINT PrimitiveIDCount = 0;

UPrimitiveComponent::UPrimitiveComponent()
{
	PrimitiveID = PrimitiveIDCount++;
	bIsPrimitive=true;


}

UPrimitiveComponent::~UPrimitiveComponent()
{

}

void UPrimitiveComponent::Register()
{
	USceneComponent::Register();

	std::shared_ptr<FPrimitiveSceneProxy> PrimitiveSceneProxy = CreateSceneProxy();
	FScene::AddPrimitive_GameThread(PrimitiveID, PrimitiveSceneProxy, GetComponentTransform());
}
