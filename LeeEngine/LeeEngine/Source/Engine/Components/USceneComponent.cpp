// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "USceneComponent.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Engine/UEditorEngine.h"

USceneComponent::USceneComponent()
{
	UpdateComponentToWorld();
}

void USceneComponent::Register()
{
	UActorComponent::Register();

	for(const auto& Child : AttachChildren)
	{
		Child->Register();
	}

	Make_Transform_Dirty()
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

void USceneComponent::SetRelativeRotation(const DirectX::XMFLOAT3& NewRelRotationPitchYawRoll)
{
	
	XMVECTOR NewQuat = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(NewRelRotationPitchYawRoll.x),
		XMConvertToRadians(NewRelRotationPitchYawRoll.y),
		XMConvertToRadians(NewRelRotationPitchYawRoll.z)
	);
	SetRelativeRotation(NewQuat);

}

void USceneComponent::SetRelativeRotation(const DirectX::XMFLOAT4& NewRotation)
{
	RelativeRotation = NewRotation;

	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeRotation(const DirectX::XMVECTOR& NewRelRotation)
{
	XMQuaternionNormalize(NewRelRotation);
	XMStoreFloat4(&RelativeRotation,NewRelRotation);

	UpdateComponentToWorld();
}


void USceneComponent::SetRelativeScale3D(const DirectX::XMFLOAT3& NewRelScale3D)
{
	RelativeScale3D = NewRelScale3D;
	UpdateComponentToWorld();
}

void USceneComponent::AddWorldOffset(const XMFLOAT3& DeltaLocation) 
{
	XMFLOAT3 CurrentComponentTranslation = GetComponentTransform().GetTranslation();
	XMVECTOR NewWorldLocationVec = XMVectorAdd(XMLoadFloat3(&DeltaLocation), XMLoadFloat3(&CurrentComponentTranslation)); 
	XMFLOAT3 NewWorldLocation;
	
	XMStoreFloat3(&NewWorldLocation, NewWorldLocationVec);
	SetWorldLocation(NewWorldLocation);
}

void USceneComponent::AddWorldRotation(const XMFLOAT3& DeltaRotation)
{
	XMVECTOR DeltaRotationQuat = XMQuaternionRotationRollPitchYaw(
			 XMConvertToRadians(DeltaRotation.x),
		 XMConvertToRadians(DeltaRotation.y),
		XMConvertToRadians(DeltaRotation.z)
	);

	XMVECTOR ComponentRotationQuat = XMQuaternionIdentity();
	//if(GetAttachParent())
	//{
	//	ComponentRotationQuat = GetAttachParent()->GetSocketTransform(GetAttachSocketName()).GetRotationQuat//();
	//}
	//else
	//{
	//	ComponentRotationQuat = XMQuaternionIdentity();//XMLoadFloat4(&RelativeRotation);
	//}

	XMVECTOR NewDeltaWorldRotationQuat = XMQuaternionMultiply(ComponentRotationQuat,DeltaRotationQuat);
	XMVECTOR NewWorldRotationQuat = XMQuaternionMultiply(GetComponentTransform().GetRotationQuat(),NewDeltaWorldRotationQuat);
	SetWorldRotation(NewWorldRotationQuat);
}

void USceneComponent::SetWorldLocation(const XMFLOAT3& NewLocation)
{
	XMFLOAT3 NewRelLocation = NewLocation;

	if(GetAttachParent())
	{
		// TODO: 
		FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
		NewRelLocation = ParentToWorld.InverseTransformPosition(NewLocation);
	}
	SetRelativeLocation(NewRelLocation);
}

void USceneComponent::SetWorldRotation(const XMVECTOR& NewRotation)
{
	XMVECTOR NewRelRotation = GetRelativeRotationFromWorld(NewRotation);
	
	SetRelativeRotation(NewRelRotation);
	
}


FTransform& USceneComponent::GetSocketTransform(const std::string& InSocketName)
{
	return ComponentToWorld;
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
	FTransform RelTransform = FTransform{XMLoadFloat4(&RelativeRotation)
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

XMVECTOR USceneComponent::GetRelativeRotationFromWorld(const XMVECTOR& NewWorldRotation)
{
	XMVECTOR NewRelRotation = NewWorldRotation;

	if(GetAttachParent())
	{
		const FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());

		// TODO: 언리얼엔진 에서는 NegativeScale 에 대한 처리도 진행

		const XMVECTOR ParentToWorldQuat = ParentToWorld.GetRotationQuat();
		XMVECTOR Inv = XMQuaternionInverse(ParentToWorldQuat);
		XMVECTOR NewRelQuat =  (XMQuaternionMultiply(NewWorldRotation,Inv)) ;

		// Rel 연산 확인용
		//XMVECTOR WorldTest = XMQuaternionMultiply(ParentToWorldQuat, NewRelQuat);

		NewRelRotation = NewRelQuat;
	}

	return NewRelRotation;
}
