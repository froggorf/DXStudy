#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"

#include <stack>

#include "UAnimSequence.h"
#include "Bone.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/Misc/QueuedThreadPool.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/RenderCore/renderingthread.h"

UAnimInstance::UAnimInstance()
{
	DeltaTime = 0.0f;
}


void UAnimInstance::BeginPlay()
{
	UObject::BeginPlay();

	NativeInitializeAnimation();
}

void UAnimInstance::NativeInitializeAnimation()
{
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
}

void UAnimInstance::UpdateAnimation(float dt)
{

}

void UAnimInstance::Tick(float DeltaSeconds)
{
	DeltaTime = DeltaSeconds;
	static float DefaultSpeed = 30;
	CurrentTime = CurrentTime + DeltaSeconds * DefaultSpeed;

	static float TicksPerSecondTime = 1.0f/30* DefaultSpeed;
	if(LatestUpdateTime + TicksPerSecondTime < CurrentTime)
	{		
		LatestUpdateTime = CurrentTime;

		NativeUpdateAnimation(DeltaSeconds);
		UpdateAnimation(DeltaSeconds);
	}

}



AActor* UAnimInstance::TryGetPawnOwner() const
{
	if (CurrentSkeletalMeshComponent)
	{
		return CurrentSkeletalMeshComponent->GetOwner();
	}
	return nullptr;
}

void UAnimInstance::LayeredBlendPerBone(const std::vector<XMMATRIX>& BasePose, const std::vector<XMMATRIX>& BlendPose,
	const std::string& TargetBoneName, float BlendWeights, std::vector<XMMATRIX>& OutMatrices)
{
	auto BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
	if(BoneHierarchyMap.contains(GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()))
	{
		std::vector<FPrecomputedBoneData> BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];

		// BlendPose의 TargetBone을 BasePose의 TargetBone 좌표계로 바꾸기 위한 행렬 계산
		XMMATRIX ToBaseAnimTargetBoneMatrix = XMMatrixIdentity();
		for (int i = 0; i < BoneHierarchy.size(); ++i)
		{
			if (BoneHierarchy[i].BoneName == TargetBoneName)
			{
				ToBaseAnimTargetBoneMatrix = XMMatrixInverse(nullptr, BlendPose[BoneHierarchy[i].BoneInfo.id]) * BasePose[BoneHierarchy[i].BoneInfo.id];
				break;
			}
		}

		// 본의 계층을 돌면서 해당 본의 부모 중 TargetBoneName의 본이 있을 경우 블렌딩
		for(int i = 0; i < BoneHierarchy.size(); ++i)
		{
			bool bHasTargetParentBone = false;
			int CurrentCheckParentIndex = BoneHierarchy[i].ParentIndex;
			while(true)
			{
				if(CurrentCheckParentIndex >= 0)
				{
					// 부모 중 타겟 본이 존재
					if(BoneHierarchy[CurrentCheckParentIndex].BoneName == TargetBoneName)
					{
						bHasTargetParentBone = true;
						break;
					}
					else
					{
						CurrentCheckParentIndex = BoneHierarchy[CurrentCheckParentIndex].ParentIndex;
						continue;
					}
				}
				break;
			}
			int CurrentBoneIndex = BoneHierarchy[i].BoneInfo.id;
			// 본의 계층을 돌면서 해당 본의 부모 중 TargetBoneName의 본이 있을 경우 블렌딩
			if(0<= CurrentBoneIndex && CurrentBoneIndex < MAX_BONES)
			{
				if(bHasTargetParentBone)
				{
					XMMatrixLerp( BasePose[CurrentBoneIndex], BlendPose[CurrentBoneIndex] *  ToBaseAnimTargetBoneMatrix, BlendWeights, OutMatrices[CurrentBoneIndex]);
				}
				else
				{
					OutMatrices[CurrentBoneIndex] = BasePose[CurrentBoneIndex];
				}	
			}

		}

	}

}

/*


 */