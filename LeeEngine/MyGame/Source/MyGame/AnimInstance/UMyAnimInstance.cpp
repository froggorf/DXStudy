
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
		UpdateAnimation(DeltaSeconds);
		
	}
}

void UMyAnimInstance::UpdateAnimation(float dt)
{
	UAnimInstance::UpdateAnimation(dt);
	

	if(GetSkeletalMeshComponent()&&BS_Paladin_IdleWalkRun)
	{
		std::vector<XMMATRIX> FinalBoneMatrices(MAX_BONES);	
		
		static float x = 0.0f;
		static int a = 0;
		static bool b = false;
		++a;
		if(a % 10 == 0)
		{
			{
				x = b? x+0.5f : x-0.5f;
				if(x < -180.0f)
				{
					x = -180.0f;
					b = 1-b;
				}
				if(x>180.0f)
				{
					x = 180.0f;
					b=1-b;
				}
			}	
		}
		
		static float y = 600.0f;

		BS_Paladin_IdleWalkRun->GetAnimationBoneMatrices(XMFLOAT2{x,y}, CurrentTime,FinalBoneMatrices);
		
		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , FinalBoneMatrices);
	}	
}
