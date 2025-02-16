// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "USceneComponent.h"


USceneComponent::USceneComponent()
{
}

void USceneComponent::SetupAttachment(USceneComponent* InParent, std::string_view InSocketName)
{
	if( InParent == AttachParent.get() && InSocketName == AttachSocketName)
	{
		// 이미 부착된 상태
		return;
	}

	if(InParent == this)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << "Warning : can't attach to itself" << std::endl;
		return;
#endif		
	}

	if( InParent->AttachParent.get() == this)
	{
#if defined(DEBUG) || defined(_DEBUG)
		std::cout << "Warning : Setting up Attachment create a cycle !" << std::endl;
		return;
#endif
	}

	SetAttachParent(InParent);
	SetAttachSocketName(InSocketName);
	const std::vector<std::shared_ptr<USceneComponent>>& ParentAttachChildren = InParent->AttachChildren;
	std::shared_ptr<USceneComponent> thisPtr = std::make_shared<USceneComponent>(*this);
	if(std::find(AttachChildren.begin(),AttachChildren.end(), thisPtr) == AttachChildren.end())
	{
		InParent->AttachChildren.push_back(thisPtr);	
	}
}

void USceneComponent::TestDraw()
{
	TestDrawComponent();

	for(const auto& ChildComponent : AttachChildren)
	{
		ChildComponent->TestDraw();
	}
}

void USceneComponent::SetAttachParent(USceneComponent* NewAttachParent)
{
	AttachParent = std::shared_ptr<USceneComponent>(NewAttachParent);
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::SetAttachSocketName(std::string_view NewSocketName)
{
	AttachSocketName = NewSocketName.data();
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::UpdateComponentToWorldWithParent(USceneComponent* Parent, std::string_view SocketName,
	const DirectX::XMVECTOR& RelativeRotationQuat)
{
	// 부모의 위치 받아와서
	// 내 Relative Transform 곱해줘서
	// 내 ComponentToWorld에 갱신해주기

	//자식들 loop 돌면서 자식들 ComponentToWorld 갱신
}
