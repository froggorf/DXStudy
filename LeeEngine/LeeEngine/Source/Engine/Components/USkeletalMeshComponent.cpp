// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "USkeletalMeshComponent.h"

#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UShapeComponent.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"

USkeletalMeshComponent::USkeletalMeshComponent()
{
	Rename("SkeletalMeshComponent_" + std::to_string(ComponentID));

}

void USkeletalMeshComponent::BeginPlay()
{
	USkinnedMeshComponent::BeginPlay();

	if (AnimInstance)
	{
		AnimInstance->BeginPlay();
	}
}

void USkeletalMeshComponent::Register()
{
	USkinnedMeshComponent::Register();

	if (AnimInstance)
	{
		AnimInstance->Tick(0.0f);
	}
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> USkeletalMeshComponent::CreateSceneProxy()
{
	if (nullptr == SkeletalMesh)
	{
		return {};
	}

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;

	UINT MeshCount = SkeletalMesh->GetSkeletalMeshRenderData()->MeshCount;
	for (UINT i = 0; i < MeshCount; ++i)
	{
		auto SceneProxy = std::make_shared<FSkeletalMeshSceneProxy>(PrimitiveID, i, SkeletalMesh);
		if (SceneProxy->GetMaterialInterface())
		{
			SceneProxies.emplace_back(SceneProxy);
		}
	}

	return SceneProxies;
}

bool USkeletalMeshComponent::SetSkeletalMesh(const std::shared_ptr<USkeletalMesh>& NewMesh)
{
	if (nullptr == NewMesh)
	{
		MY_LOG("SetStaticMesh", EDebugLogLevel::DLL_Warning, "nullptr StaticMesh");
		return false;
	}
	if (NewMesh.get() == GetSkeletalMesh().get())
	{
		return false;
	}

	SkeletalMesh = NewMesh;
	SkeletalMeshName = SkeletalMesh->GetName();
	RegisterSceneProxies();

	UpdateComponentToWorld();
	return true;
}

FTransform USkeletalMeshComponent::GetSocketTransform(const std::string& InSocketName)
{
	if (!AnimInstance && !GetSkeletalMesh())
	{
		return GetComponentTransform();
	}

	FTransform ReturnTransform = GetComponentTransform();

	std::map<std::string,std::vector<FPrecomputedBoneData>>& BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();

	if (BoneHierarchyMap.contains(SkeletalMeshName))
	{
		std::vector<FPrecomputedBoneData>& BoneHierarchy = BoneHierarchyMap[SkeletalMeshName];
		auto TargetSocket = std::ranges::find_if(BoneHierarchy, [InSocketName](const FPrecomputedBoneData& Data)
		{
			return Data.BoneName == InSocketName;
		});

		if (TargetSocket == BoneHierarchy.end())
		{
			return GetComponentTransform();
		}
		int BoneIdx = static_cast<int>(std::distance(BoneHierarchy.begin(), TargetSocket));

		XMVECTOR OutLoc, OutRot, OutScale;
		XMMATRIX Matrix = AnimInstance->GetGlobalBoneTransforms()[BoneIdx] * ReturnTransform.ToMatrixWithScale();
		bool bDecompose = XMMatrixDecompose(&OutScale, &OutRot, &OutLoc, Matrix);
		if (bDecompose)
		{
			XMStoreFloat3(&ReturnTransform.Translation, OutLoc);
			XMStoreFloat4(&ReturnTransform.Rotation, OutRot);
			XMStoreFloat3(&ReturnTransform.Scale3D, OutScale);
		}
	}


	return ReturnTransform;
}

void USkeletalMeshComponent::SetAnimInstanceClass(const std::string& InAnimInstanceClass)
{
	if (const UObject* AnimDefaultObject = GetDefaultObject(InAnimInstanceClass))
	{
		if (std::shared_ptr<UAnimInstance> NewAnimInstance = std::dynamic_pointer_cast<UAnimInstance>(AnimDefaultObject->CreateInstance()))
		{
			NewAnimInstance->SetSkeletalMeshComponent(this);
			AnimInstance = NewAnimInstance;
			AnimInstance->Register();
			return;
		}
		MY_LOG("SetAnimInstanceClass", EDebugLogLevel::DLL_Warning, "Invalid AnimInstanceClass");
	}
}

void USkeletalMeshComponent::TickComponent(float DeltaSeconds)
{
	USkinnedMeshComponent::TickComponent(DeltaSeconds);

	if (AnimInstance)
	{
		AnimInstance->Tick(DeltaSeconds);
	}

	UpdateComponentToWorld();
	if (GetBodyInstance())
	{
		GetBodyInstance()->TickComponent(0.0f);
	}
}

void USkeletalMeshComponent::Tick_Editor(float DeltaSeconds)
{
	USkinnedMeshComponent::Tick_Editor(DeltaSeconds);

	UpdateComponentToWorld();
}
