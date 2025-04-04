
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
	

	if(GetSkeletalMeshComponent()&&BS_Paladin_IdleWalkRun && TestComp)
	{
		std::vector<XMMATRIX> FinalBoneMatrices(MAX_BONES);	
		
		BS_Paladin_IdleWalkRun->GetAnimationBoneMatrices(XMFLOAT2{TestComp->TestAngle,TestComp->TestSpeed}, CurrentTime,FinalBoneMatrices);
		
		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}
