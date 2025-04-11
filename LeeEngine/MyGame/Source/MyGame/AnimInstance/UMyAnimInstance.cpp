#include "CoreMinimal.h"
#include "UMyAnimInstance.h"

#include <Engine/RenderCore/EditorScene.h>

#include "../Actor/ATestActor2.h"
#include "Engine/Components/USkeletalMeshComponent.h"

UMyAnimInstance* UMyAnimInstance::MyAnimInstance;

UMyAnimInstance::UMyAnimInstance()
{
	if(const std::shared_ptr<UBlendSpace> BlendSpace = UBlendSpace::GetAnimationAsset("BS_MyUEFN_LocomotionRPG"))
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
	MyAnimInstance = this;
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

		// BlendSpace_Locomotion
		std::vector<XMMATRIX> BS_IdleWalkRunMatrices(MAX_BONES,XMMatrixIdentity());
		std::vector<FAnimNotifyEvent> BS_Notifies;
		BS_MyUEFN_Locomotion->GetAnimationBoneMatrices(XMFLOAT2{0.0f,TestComp->TestSpeed}, CurrentTime,BS_IdleWalkRunMatrices,BS_Notifies);
		for(int i = 0; i < BS_Notifies.size(); ++i)
		{
			BS_Notifies[i].Notify->Notify();
		}

		// 애니메이션 시퀀스 계산
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
		std::vector<XMMATRIX> ResultMatrices(MAX_BONES, XMMatrixIdentity());
		LayeredBlendPerBone(BS_IdleWalkRunMatrices, AS_Matrices, "spine_01", 1.0f, ResultMatrices);


		// 몽타쥬 연결
		{
			std::string SlotName = "DefaultSlot";
			std::vector<XMMATRIX> MontageMatrices(MAX_BONES,XMMatrixIdentity());
			for(const auto& MontageInstance : MontageInstances)
			{
				if(MontageInstance->Montage->SlotName == SlotName)
				{
					MontageMatrices = MontageInstance->MontageBones;

					float BlendInBlendTime = MontageInstance->Montage->BlendIn.GetBlendTime();
					float BlendOutBlendTime = MontageInstance->Montage->BlendOut.GetBlendTime();

					float StartTimeFrame, EndTimeFrame;
					MontageInstance->Montage->GetSectionStartAndEndTime(MontageInstance->GetPosition(), StartTimeFrame,EndTimeFrame);
					float StartTime = StartTimeFrame / 30; // 30FPS
					float EndTime = EndTimeFrame/30;


					if(MontageInstance->CurrentPlayTime < BlendInBlendTime)
					{
						const FRichCurve& BlendInCurve = MontageInstance->Montage->BlendIn.GetCurve()->GetRichCurve();
						float NormalizedTime = MontageInstance->CurrentPlayTime / BlendInBlendTime; // 0 ~ 1 의 값
						float CurveValue = BlendInCurve.Eval(NormalizedTime);
						for(int i = 0; i < MAX_BONES; ++i)
						{
							XMMatrixLerp(ResultMatrices[i], MontageMatrices[i], CurveValue, ResultMatrices[i]);	
						}
					}
					else if(EndTime - BlendOutBlendTime<= MontageInstance->CurrentPlayTime && MontageInstance->CurrentPlayTime < EndTime)
					{
						const FRichCurve& BlendOutCurve = MontageInstance->Montage->BlendOut.GetCurve()->GetRichCurve();
						float NormalizedTime = (EndTime - MontageInstance->CurrentPlayTime) / BlendOutBlendTime; // 1~0의 값
						float CurveValue = BlendOutCurve.Eval(NormalizedTime);
						for(int i = 0; i < MAX_BONES; ++i)
						{
							XMMatrixLerp(ResultMatrices[i],MontageMatrices[i], CurveValue, ResultMatrices[i]);	
						}
					}
					else
					{
						ResultMatrices = MontageMatrices;
					}

				}
			}
		}




		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID() , ResultMatrices);
	}	
}
