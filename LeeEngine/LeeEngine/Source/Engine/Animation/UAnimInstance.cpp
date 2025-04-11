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
	for(int i = 0; i < MontageInstances.size(); ++i)
	{
		MontageInstances[i]->Play();

		// 몽타쥬 재생 종료됨
		if(!MontageInstances[i]->bIsPlaying)
		{
			MontageInstances.erase(MontageInstances.begin() + i);
			--i;
		}
	}
	
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

void UAnimInstance::Montage_Play(std::shared_ptr<UAnimMontage> MontageToPlay, float InTimeToStartMontageAt)
{
	if(!MontageToPlay || MontageToPlay->GetPlayLength() <= 0.0f)
	{
		return;
	}

	// 이미 해당 몽타쥬가 재생중인지 확인
	for(const auto& MontageInstance : MontageInstances)
	{
		if(MontageInstance->Montage == MontageToPlay)
		{
			MontageInstance->SetPosition(std::clamp(InTimeToStartMontageAt, 0.0f, MontageToPlay->GetPlayLength()));
			return;
		}
	}

	std::shared_ptr<FAnimMontageInstance> NewInstance = std::make_shared<FAnimMontageInstance>(this);
	NewInstance->Montage = MontageToPlay;
	NewInstance->SetPosition(std::clamp(InTimeToStartMontageAt, 0.0f, MontageToPlay->GetPlayLength()));
	NewInstance->CurrentPlayTime = 0.0f;
	MontageInstances.emplace_back(NewInstance);
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

void UAnimInstance::PlayMontage(const std::string& SlotName, std::vector<XMMATRIX>& OriginMatrices)
{
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
					XMMatrixLerp(OriginMatrices[i], MontageMatrices[i], CurveValue, OriginMatrices[i]);	
				}
			}
			else if(EndTime - BlendOutBlendTime<= MontageInstance->CurrentPlayTime && MontageInstance->CurrentPlayTime < EndTime)
			{
				const FRichCurve& BlendOutCurve = MontageInstance->Montage->BlendOut.GetCurve()->GetRichCurve();
				float NormalizedTime = (EndTime - MontageInstance->CurrentPlayTime) / BlendOutBlendTime; // 1~0의 값
				float CurveValue = BlendOutCurve.Eval(NormalizedTime);
				for(int i = 0; i < MAX_BONES; ++i)
				{
					XMMatrixLerp(OriginMatrices[i],MontageMatrices[i], CurveValue, OriginMatrices[i]);	
				}
			}
			else
			{
				OriginMatrices = MontageMatrices;
			}

		}
	}


}
/*


 */
