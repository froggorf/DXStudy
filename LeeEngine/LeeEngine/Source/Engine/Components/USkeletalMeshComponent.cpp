﻿// 03.24
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "USkeletalMeshComponent.h"

#include "Engine/Mesh/USkeletalMesh.h"
#include "Engine/Physics/UShapeComponent.h"
#include "Engine/SceneProxy/FSkeletalMeshSceneProxy.h"
#include "Engine/RenderCore/EditorScene.h"

USkeletalMeshComponent::USkeletalMeshComponent()
{
	Rename("SkeletalMeshComponent_" + std::to_string(ComponentID));
	AnimInstance = std::make_shared<UAnimInstance>();
	AnimInstance->SetSkeletalMeshComponent(this);
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
		AnimInstance->UpdateAnimation(0.0f);
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
		SceneProxies.emplace_back(SceneProxy);
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
		XMVECTOR OutLoc, OutRot, OutScale;
		int BoneIdx = std::distance(BoneHierarchy.begin(), TargetSocket);
		// 누적 행렬 계산
		FTransform AnimTransform;
		if (XMMatrixDecompose(&OutScale, &OutRot, &OutLoc, AnimInstance->GetLastFrameAnimMatrices()[BoneIdx]))
		{
			XMStoreFloat3(&AnimTransform.Translation, OutLoc);
			XMStoreFloat4(&AnimTransform.Rotation, OutRot);
			XMStoreFloat3(&AnimTransform.Scale3D, OutScale);
		}

		XMMATRIX BoneOffset = XMMatrixIdentity();
		while (BoneIdx >= 0)
		{
			BoneOffset = XMMatrixMultiply(BoneOffset, BoneHierarchy[BoneIdx].BoneTransform);
			BoneIdx = BoneHierarchy[BoneIdx].ParentIndex;
		}
	
		bool bDecompose = XMMatrixDecompose(&OutScale, &OutRot, &OutLoc, BoneOffset);
		FTransform BoneTransform;
		if (bDecompose)
		{
			XMStoreFloat3(&BoneTransform.Translation, OutLoc);
			XMStoreFloat4(&BoneTransform.Rotation, OutRot);
			XMStoreFloat3(&BoneTransform.Scale3D, OutScale);
		}


		ReturnTransform = ReturnTransform * AnimTransform* BoneTransform;
	}

	return ReturnTransform;
}

void USkeletalMeshComponent::SetAnimInstanceClass(const std::string& InAnimInstanceClass)
{
	if (const UObject* AnimDefaultObject = GetDefaultObject(InAnimInstanceClass))
	{
		if (std::shared_ptr<UAnimInstance> NewAnimInstance = std::dynamic_pointer_cast<UAnimInstance>(AnimDefaultObject->CreateInstance()))
		{
			AnimInstance = NewAnimInstance;
			AnimInstance->SetSkeletalMeshComponent(this);
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
