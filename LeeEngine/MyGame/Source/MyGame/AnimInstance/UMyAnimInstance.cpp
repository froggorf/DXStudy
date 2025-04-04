
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "Engine/Components/USkeletalMeshComponent.h"



UMyAnimInstance::UMyAnimInstance()
{
	if(const std::shared_ptr<UBlendSpace> BlendSpace = UBlendSpace::GetAnimationAsset("BS_Paladin_IdleWalkRun"))
	{
		BS_Paladin_IdleWalkRun = BlendSpace;
	}
	if (const std::shared_ptr<UAimOffsetBlendSpace> AimOffset = UAimOffsetBlendSpace::GetAnimationAsset("AO_Paladin_Stand"))
	{
		AO_Paladin_Stand = AimOffset;
	}

}

void UMyAnimInstance::BeginPlay()
{
	UAnimInstance::BeginPlay();
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	UAnimInstance::NativeInitializeAnimation();
	
	if(AActor* OwnerActor = GetSkeletalMeshComponent()->GetOwner())
	{
		if(ATestActor2* TestActor = dynamic_cast<ATestActor2*>(OwnerActor))
		{
			if(const std::shared_ptr<UTestComponent>& OwnerTestComp = std::dynamic_pointer_cast<UTestComponent>(TestActor->FindComponentByClass("UTestComponent")))
			{
				TestComp = OwnerTestComp;
			}
		}
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if(TestComp)
	{
		
		
	}
}

void UMyAnimInstance::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);
	

	if(GetSkeletalMeshComponent()&&BS_Paladin_IdleWalkRun && TestComp && AO_Paladin_Stand)
	{
		std::vector<XMMATRIX> FinalBoneMatrices(MAX_BONES, XMMatrixIdentity());	

		std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES);
		BS_Paladin_IdleWalkRun->GetAnimationBoneMatrices(XMFLOAT2{TestComp->TestAngle,TestComp->TestSpeed}, CurrentTime,BS_IdleWalkRunMatrices);

		std::vector<XMMATRIX> AO_StandMatrices(MAX_BONES);
		AO_Paladin_Stand->GetAnimationBoneMatrices(XMFLOAT2{TestComp->TestAimOffsetAngleX,TestComp->TestAimOffsetAngleY}, CurrentTime,AO_StandMatrices);

		

		std::vector<XMMATRIX> ResultMatrices(MAX_BONES);
		std::string TargetBoneName = "Spine";
		auto BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
		if(BoneHierarchyMap.contains(GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()))
		{
			std::vector<FPrecomputedBoneData> BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];
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
				if(0<= CurrentBoneIndex && CurrentBoneIndex < MAX_BONES)
				{
					if(bHasTargetParentBone)
					{


						ResultMatrices[CurrentBoneIndex] = AO_StandMatrices[CurrentBoneIndex];
					}
					else
					{
						ResultMatrices[CurrentBoneIndex] = BS_IdleWalkRunMatrices[CurrentBoneIndex];
					}	
				}
				
			}
			
		}
		
		

		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , ResultMatrices);
	}	
}
