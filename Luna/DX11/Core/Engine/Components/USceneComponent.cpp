// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "USceneComponent.h"


USceneComponent::USceneComponent()
{
	UpdateComponentToWorld();
}

void USceneComponent::SetupAttachment(const std::shared_ptr<USceneComponent>& InParent, std::string_view InSocketName)
{
	if( InParent.get() == AttachParent.get() && InSocketName == AttachSocketName)
	{
		// 이미 부착된 상태
		return;
	}

	if(InParent.get() == this)
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
	std::shared_ptr<USceneComponent> thisPtr = shared_from_this(); // std::enable_shared_from_this<USceneComponent>
	if(std::find(AttachChildren.begin(),AttachChildren.end(), thisPtr) == AttachChildren.end())
	{
		InParent->AttachChildren.push_back(thisPtr);	
	}

	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeLocation(const DirectX::XMFLOAT3& NewRelLocation)
{
	RelativeLocation = NewRelLocation;
	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeRotation(const DirectX::XMFLOAT3& NewRelRotation)
{
	RelativeRotation = NewRelRotation;
	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeScale3D(const DirectX::XMFLOAT3& NewRelScale3D)
{
	RelativeScale3D = NewRelScale3D;
	UpdateComponentToWorld();
}

void USceneComponent::TestDraw()
{
	std::cout << typeid(this).name() << std::endl;
	TestDrawComponent();

	//int ComponentCount = AttachChildren.size();
	//for(int index = 0; index < ComponentCount; ++index)
	//{
	//	AttachChildren[index]->TestDraw();
	//}
	for(const auto& ChildComponent : AttachChildren)
	{
		ChildComponent->TestDraw();
	}
}

void USceneComponent::TestDrawComponent()
{
}

void USceneComponent::SetAttachParent(const std::shared_ptr<USceneComponent>& NewAttachParent)
{
	AttachParent = NewAttachParent;
	std::cout << AttachParent.use_count();
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::SetAttachSocketName(std::string_view NewSocketName)
{
	AttachSocketName = NewSocketName.data();
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::UpdateComponentToWorldWithParent(const std::shared_ptr<USceneComponent>& Parent,
	std::string_view SocketName)
{
	// 언리얼엔진의 내부에선
	// 행렬을 통한 위치 계산이 아닌
	// FTransform * 연산자를 통해 부분적인 연산을 적용하므로 해당 방식을 채용
	FTransform RelTransform = FTransform{DirectX::XMQuaternionRotationRollPitchYaw(RelativeRotation.x,RelativeRotation.y,RelativeRotation.z)
		, RelativeLocation,RelativeScale3D};

	if(nullptr == Parent)
	{
		ComponentToWorld = RelTransform;
	}
	else
	{
		ComponentToWorld = Parent->ComponentToWorld * RelTransform;	
	}

	

	for(const auto& Child : AttachChildren)
	{
		Child->UpdateComponentToWorld();
	}
}
