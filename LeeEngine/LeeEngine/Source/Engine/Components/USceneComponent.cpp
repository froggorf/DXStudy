// 02.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "CoreMinimal.h"
#include "USceneComponent.h"
#include "Engine/UEditorEngine.h"
#include "Engine/RenderCore/EditorScene.h"

USceneComponent::USceneComponent()
{
	UpdateComponentToWorld();
}

void USceneComponent::Register()
{
	UActorComponent::Register();

	for (const auto& Child : AttachChildren)
	{
		Child->Register();
	}

	Make_Transform_Dirty()
}

void USceneComponent::BeginPlay()
{
	UActorComponent::BeginPlay();

	for (const auto& ChildComponent : AttachChildren)
	{
		ChildComponent->BeginPlay();
	}
}

void USceneComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);

	for (const auto& ChildComponent : AttachChildren)
	{
		ChildComponent->TickComponent(DeltaSeconds);
	}
}

void USceneComponent::Tick_Editor(float DeltaSeconds)
{
	for (const auto& ChildComponent : AttachChildren)
	{
		ChildComponent->Tick_Editor(DeltaSeconds);
	}
}

void USceneComponent::SetupAttachment(const std::shared_ptr<USceneComponent>& InParent, std::string_view InSocketName)
{
	if (InParent.get() == AttachParent.get() && InSocketName == AttachSocketName)
	{
		// 이미 부착된 상태
		return;
	}

	if (InParent.get() == this)
	{
		MY_LOG("SetupAttachment", EDebugLogLevel::DLL_Error, "Can't attach to itself");
		return;
	}

	if (InParent->AttachParent.get() == this)
	{
		MY_LOG("SetupAttachment", EDebugLogLevel::DLL_Error, "Setting up Attachment create a cycle !");
		return;
	}

	SetAttachParent(InParent);
	SetAttachSocketName(InSocketName);
	const std::vector<std::shared_ptr<USceneComponent>>& ParentAttachChildren = InParent->AttachChildren;
	std::shared_ptr<USceneComponent>                     thisPtr              = shared_from_this(); // std::enable_shared_from_this<USceneComponent>
	if (std::find(AttachChildren.begin(), AttachChildren.end(), thisPtr) == AttachChildren.end())
	{
		thisPtr->SetOwner(InParent->GetOwner());
		InParent->AttachChildren.push_back(thisPtr);
	}

	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeLocation(const XMFLOAT3& NewRelLocation)
{
	RelativeLocation = NewRelLocation;
	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeRotation(const XMFLOAT3& NewRelRotationPitchYawRoll)
{
	XMVECTOR NewQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(NewRelRotationPitchYawRoll.x), XMConvertToRadians(NewRelRotationPitchYawRoll.y), XMConvertToRadians(NewRelRotationPitchYawRoll.z));
	SetRelativeRotation(NewQuat);
}

void USceneComponent::SetRelativeRotation(const XMFLOAT4& NewRotation)
{
	RelativeRotation = NewRotation;

	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeRotation(const XMVECTOR& NewRelRotation)
{
	XMQuaternionNormalize(NewRelRotation);
	XMStoreFloat4(&RelativeRotation, NewRelRotation);

	UpdateComponentToWorld();
}

void USceneComponent::SetRelativeScale3D(const XMFLOAT3& NewRelScale3D)
{
	RelativeScale3D = NewRelScale3D;
	UpdateComponentToWorld();
}

void USceneComponent::AddWorldOffset(const XMFLOAT3& DeltaLocation)
{
	XMFLOAT3 CurrentComponentTranslation = GetComponentTransform().GetTranslation();
	XMVECTOR NewWorldLocationVec         = XMVectorAdd(XMLoadFloat3(&DeltaLocation), XMLoadFloat3(&CurrentComponentTranslation));
	XMFLOAT3 NewWorldLocation;

	XMStoreFloat3(&NewWorldLocation, NewWorldLocationVec);
	SetWorldLocation(NewWorldLocation);
}

void USceneComponent::AddWorldRotation(const XMFLOAT3& DeltaRotation)
{
	XMVECTOR DeltaRotationQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(DeltaRotation.x), XMConvertToRadians(DeltaRotation.y), XMConvertToRadians(DeltaRotation.z));

	XMVECTOR ComponentRotationQuat = XMQuaternionIdentity();

	XMVECTOR NewDeltaWorldRotationQuat = XMQuaternionMultiply(ComponentRotationQuat, DeltaRotationQuat);
	XMVECTOR NewWorldRotationQuat      = XMQuaternionMultiply(GetComponentTransform().GetRotationQuat(), NewDeltaWorldRotationQuat);
	SetWorldRotation(NewWorldRotationQuat);
}

void USceneComponent::SetWorldLocation(const XMFLOAT3& NewLocation)
{
	XMFLOAT3 NewRelLocation = NewLocation;

	if (GetAttachParent())
	{
		FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
		NewRelLocation           = ParentToWorld.InverseTransformPosition(NewLocation);
	}
	SetRelativeLocation(NewRelLocation);
}

void USceneComponent::SetWorldRotation(const XMVECTOR& NewRotation)
{
	XMVECTOR NewRelRotation = GetRelativeRotationFromWorld(NewRotation);

	SetRelativeRotation(NewRelRotation);
}

FTransform USceneComponent::GetSocketTransform(const std::string& InSocketName)
{
	return ComponentToWorld;
}

XMFLOAT3 USceneComponent::GetForwardVector()
{
	XMVECTOR WorldFront = XMVectorSet(0,0,1,0);
	XMFLOAT4 CurRot = GetWorldRotation();
	XMVECTOR ComponentFront = XMVector3Rotate(WorldFront, XMLoadFloat4(&CurRot));
	XMFLOAT3 ComponentFrontVector;
	XMStoreFloat3(&ComponentFrontVector,ComponentFront);
	return ComponentFrontVector;
}

#ifdef WITH_EDITOR

void USceneComponent::DrawDetailPanel(UINT ComponentDepth)
{
	if (ComponentDepth == 0)
	{
		// Imgui의 윈도우가 시작되었다고 가정하에 진행
		if (ImGui::CollapsingHeader("Transform"))
		{
			// Location
			ImGui::Text("Location");
			ImGui::SameLine(100);
			float Location[3] = {RelativeLocation.x, RelativeLocation.y, RelativeLocation.z};
			if (ImGui::InputFloat3("##Location", Location))
			{
				SetRelativeLocation(XMFLOAT3{Location[0], Location[1], Location[2]});
			}

			// Scale
			ImGui::Text("Rotation");
			ImGui::SameLine(100);
			float Rot[4] = {RelativeRotation.x,RelativeRotation.y,RelativeRotation.z,RelativeRotation.w};
			if (ImGui::InputFloat4("##Scale", Rot))
			{
				SetRelativeRotation(XMFLOAT4{Rot[0],Rot[1],Rot[2],Rot[3]});
			}

			// Scale
			ImGui::Text("Scale");
			ImGui::SameLine(100);
			float Scale[3] = {RelativeScale3D.x, RelativeScale3D.y, RelativeScale3D.z};
			if (ImGui::InputFloat3("##Scale", Scale))
			{
				SetRelativeScale3D(XMFLOAT3{Scale[0], Scale[1], Scale[2]});
			}
		}
	}

	for (const auto& ChildComponent : GetAttachChildren())
	{
		ChildComponent->DrawDetailPanel(ComponentDepth + 1);
	}
}
#endif

void USceneComponent::SetAttachParent(const std::shared_ptr<USceneComponent>& NewAttachParent)
{
	AttachParent = NewAttachParent;
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::SetAttachSocketName(std::string_view NewSocketName)
{
	AttachSocketName = NewSocketName.data();
	// 언리얼 엔진에서는 MARK_PROPERTY_DIRTY_FROM_NAME 를 통해 리플렉션 시스템을 적용
}

void USceneComponent::UpdateComponentToWorldWithParent(const std::shared_ptr<USceneComponent>& Parent, const std::string& SocketName)
{
	// 언리얼엔진의 내부에선
	// 행렬을 통한 위치 계산이 아닌
	// FTransform * 연산자를 통해 부분적인 연산을 적용하므로 해당 방식을 채용
	const FTransform& RelTransform = FTransform{XMLoadFloat4(&RelativeRotation), RelativeLocation, RelativeScale3D};

	if (nullptr == Parent)
	{
		ComponentToWorld = RelTransform;
	}
	else
	{
		ComponentToWorld = Parent->GetSocketTransform(SocketName) * RelTransform;
	}

	for (const auto& Child : AttachChildren)
	{
		Child->UpdateComponentToWorld();
	}
}

XMVECTOR USceneComponent::GetRelativeRotationFromWorld(const XMVECTOR& NewWorldRotation)
{
	XMVECTOR NewRelRotation = NewWorldRotation;

	if (GetAttachParent())
	{
		const FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());

		// TODO: 언리얼엔진 에서는 NegativeScale 에 대한 처리도 진행

		const XMVECTOR ParentToWorldQuat = ParentToWorld.GetRotationQuat();
		XMVECTOR       Inv               = XMQuaternionInverse(ParentToWorldQuat);
		XMVECTOR       NewRelQuat        = (XMQuaternionMultiply(NewWorldRotation, Inv));

		// Rel 연산 확인용
		//XMVECTOR WorldTest = XMQuaternionMultiply(ParentToWorldQuat, NewRelQuat);

		NewRelRotation = NewRelQuat;
	}

	return NewRelRotation;
}
