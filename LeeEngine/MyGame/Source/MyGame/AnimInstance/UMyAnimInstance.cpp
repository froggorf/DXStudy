
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "Engine/Components/USkeletalMeshComponent.h"



UMyAnimInstance::UMyAnimInstance()
{
	if(const std::shared_ptr<UBlendSpace> BlendSpace = UBlendSpace::GetAnimationAsset("BS_MyUEFN_Locomotion"))
	{
		BS_MyUEFN_Locomotion = BlendSpace;
	}
	if (const std::shared_ptr<UAimOffsetBlendSpace> AimOffset = UAimOffsetBlendSpace::GetAnimationAsset("AO_MyUEFN_Stand"))
	{
		AO_MyUEFN_Stand = AimOffset;
	}

	if (const std::shared_ptr<UAnimSequence> AnimSequence = UAnimSequence::GetAnimationAsset("AS_MyUEFN_Idle"))
	{
		AS_Test0 = AnimSequence;
	}
	if (const std::shared_ptr<UAnimSequence> AnimSequence = UAnimSequence::GetAnimationAsset("AS_Aim"))
	{
		AS_Test1 = AnimSequence;
	}
	if (const std::shared_ptr<UAnimSequence> AnimSequence = UAnimSequence::GetAnimationAsset("AS_Pistol"))
	{
		AS_Test2 = AnimSequence;
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
	

	if(GetSkeletalMeshComponent() && BS_MyUEFN_Locomotion && TestComp && AS_Test1 && AS_Test2)
	{
		std::vector<XMMATRIX> FinalBoneMatrices(MAX_BONES, XMMatrixIdentity());	

		std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES,XMMatrixIdentity());
		BS_MyUEFN_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{TestComp->TestAngle,TestComp->TestSpeed}, CurrentTime,BS_IdleWalkRunMatrices);

		std::vector<XMMATRIX> AS_Matrices(MAX_BONES, XMMatrixIdentity());
	
		switch(TestComp->TargetAnim)
		{
		case 0:
			AS_Test0->GetBoneTransform(CurrentTime,AS_Matrices);
		break;	
		case 1:
			AS_Test1->GetBoneTransform(CurrentTime, AS_Matrices);
		break;
		case 2:
			AS_Test2->GetBoneTransform(CurrentTime, AS_Matrices);
		break;
		default:
		break;
		}
		
		
		
		
		// 레이어 블렌딩
		std::vector<XMMATRIX> ResultMatrices(MAX_BONES);
		std::string TargetBoneName = "spine_01";
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


						ResultMatrices[CurrentBoneIndex] = AS_Matrices[CurrentBoneIndex];
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
