// 03.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UPrimitiveComponent.h"

#include "Engine/Physics/UShapeComponent.h"
#include "Engine/RenderCore/RenderingThread.h"

UINT PrimitiveIDCount = 0;

UPrimitiveComponent::UPrimitiveComponent()
{
	PrimitiveID  = PrimitiveIDCount++;
	bIsPrimitive = true;

	for (size_t i = 0; i < TempCollisionResponse.size(); ++i)
	{
		TempCollisionResponse[i] = ECollisionResponse::Ignore;
	}

}


void UPrimitiveComponent::Register()
{
	USceneComponent::Register();

	RegisterSceneProxies();


	// 지연 변수 적용
	BodyInstance = CreateBodyInstance();

	if (BodyInstance)
	{
		BodyInstance->SetObjectType(TempCollisionChannel);
		for (size_t i = 0; i < TempCollisionResponse.size(); ++i)
		{
			BodyInstance->SetResponseToChannel(static_cast<ECollisionChannel>(i), TempCollisionResponse[i]);
		}
		BodyInstance->SetupAttachment(shared_from_this(),"");

		BodyInstance->RegisterPhysics();	
	}
}

void UPrimitiveComponent::UnRegister()
{
	USceneComponent::UnRegister();

	if (BodyInstance)
	{
		BodyInstance->UnRegister();	
	}
}

void UPrimitiveComponent::RegisterSceneProxies()
{
	// 이전에 레지스터된 씬 프록시가 있다면 해당 PrimitiveID의 씬 프록시를 모두 제거하도록 요청
	if(RegisteredSceneProxyCount > 0)
	{
		FScene::KillPrimitive_GameThread(PrimitiveID);
	}

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> PrimitiveSceneProxies = CreateSceneProxy();
	for (int i = 0; i < PrimitiveSceneProxies.size(); ++i)
	{
		PrimitiveSceneProxies[i]->SetDoFrustumCulling(bDoFrustumCulling);
		PrimitiveSceneProxies[i]->SetIsMonochrome(bIsMonochromeObject);
		FScene::AddPrimitive_GameThread(PrimitiveID, PrimitiveSceneProxies[i], GetComponentTransform());
	}

	RegisteredSceneProxyCount = PrimitiveSceneProxies.size();
}


void UPrimitiveComponent::SetScalarParam(UINT MeshIndex, const std::string& ParamName, float Value) const
{
	FScene::SetMaterialScalarParam_GameThread(PrimitiveID, MeshIndex, ParamName, Value);
}

void UPrimitiveComponent::SetTextureParam(UINT MeshIndex, UINT TextureSlot, const std::shared_ptr<UTexture>& Texture) const
{
	FScene::SetTextureParam_GameThread(PrimitiveID, MeshIndex, TextureSlot, Texture);
}

std::shared_ptr<UShapeComponent> UPrimitiveComponent::CreateBodyInstance()
{
	return nullptr;
}

void UPrimitiveComponent::SetCollisionObjectType(ECollisionChannel Channel)
{
	TempCollisionChannel = Channel;
	
	if (BodyInstance)
	{
		BodyInstance->SetObjectType(Channel);
	}
}

void UPrimitiveComponent::SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse)
{
	TempCollisionResponse[static_cast<UINT>(Channel)] = NewResponse;
	if (BodyInstance)
	{
		BodyInstance->SetResponseToChannel(Channel,NewResponse);
	}
}

#ifdef WITH_EDITOR
void UPrimitiveComponent::DrawDetailPanel(UINT ComponentDepth)
{
	USceneComponent::DrawDetailPanel(ComponentDepth);
	
}
#endif

void UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled NewType)
{
	if (GetBodyInstance() && GetBodyInstance()->GetCollisionEnabled() == NewType)
	{
		return;
	}

	if (GetBodyInstance())
	{
		GetBodyInstance()->SetCollisionEnabled(NewType);
	}
}
