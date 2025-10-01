#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"
#include "Engine/Class/Framework/ACharacter.h"
#include "Engine/World/UWorld.h"

UAnimInstance::UAnimInstance()
{
	DeltaTime = 0.0f;
	FinalBoneMatrices = std::vector<XMMATRIX>(MAX_BONES, XMMatrixIdentity());
}

void UAnimInstance::BeginPlay()
{
	OwnerCharacter = dynamic_cast<ACharacter*>(GetSkeletalMeshComponent()->GetOwner());

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
	for (int i = 0; i < MontageInstances.size(); ++i)
	{
		MontageInstances[i]->Play();

		

		// 몽타쥬 재생 종료됨
		if (!MontageInstances[i]->bIsPlaying)
		{
			// 가끔 애니메이션 종료시 바로 애니메이션을 재생시킬 경우 지워지는 경우가 존재하여 해당 코드로 제어
			if (MontageInstances[i]->CurrentPlayTime == 0.0f)
			{
				 MontageInstances[i]->bIsPlaying = true;
			}
			else
			{
				MontageInstances.erase(MontageInstances.begin() + i);
				--i;
			}
			
		}
	}
}

void UAnimInstance::Tick(float DeltaSeconds)
{
	DeltaTime                 = DeltaSeconds;
	static float DefaultSpeed = 30;
	CurrentTime               = CurrentTime + DeltaSeconds * DefaultSpeed;

	static float TicksPerSecondTime = 1.0f / 30 * DefaultSpeed;
	if (LatestUpdateTime + TicksPerSecondTime < CurrentTime)
	{
		bPlayRootMotion = false;
		LatestUpdateTime = CurrentTime;

		NativeUpdateAnimation(DeltaSeconds);
		UpdateAnimation(DeltaSeconds);

		// 노티파이 실행
		for (int i = 0; i < FinalNotifies.size(); ++i)
		{
			const std::string NotifyName = FinalNotifies[i].Notify->Notify();

			const auto& Notify = NotifyEvent.find(NotifyName);
			if (Notify != NotifyEvent.end())
			{
				Notify->second.Broadcast();
			}
		}
		FinalNotifies.clear();

		/// 루트 모션 처리
		if (APlayerController* PC = GEngine->GetWorld()->GetPlayerController())
		{
			// PlayerController에 의해 조종되는 캐릭터일 경우
			bool bPlayerCharacter =  (PC->GetCharacter() && PC->GetCharacter()->GetSkeletalMeshComponent() == GetSkeletalMeshComponent());
			if (bPlayerCharacter)
			{
				PC->SetPlayRootMotion(false);
			}
			
			if (bPlayRootMotion)
			{
				XMMATRIX RootMatrixWithNoRotation = FinalBoneMatrices[0];
				RootMatrixWithNoRotation.r[0] = XMVectorSet(1, 0, 0, 0);
				RootMatrixWithNoRotation.r[1] = XMVectorSet(0, 1, 0, 0);  
				RootMatrixWithNoRotation.r[2] = XMVectorSet(0, 0, 1, 0);
				XMMATRIX RootNoRotInv = XMMatrixInverse(nullptr, RootMatrixWithNoRotation);
				
				for (size_t i = 1 ; i < MAX_BONES; ++i)
				{
					FinalBoneMatrices[i] = XMMatrixMultiply(FinalBoneMatrices[i], RootNoRotInv); 
				}

				if (bPlayerCharacter)
				{
					PC->SetPlayRootMotion(true);
				}

				if (!bBlendOut && !bUseMotionWarping)
				{
					OwnerCharacter->HandleRootMotion(FinalBoneMatrices[0]);
				}

				FinalBoneMatrices[0] = XMMatrixIdentity();	
			}	
		}
		
		// 애니메이션 데이터를 렌더링쓰레드에 전달
		FScene::UpdateSkeletalMeshAnimation_GameThread(GetSkeletalMeshComponent()->GetPrimitiveID(), FinalBoneMatrices);

		// GetSocketTransform에 사용하기 위한 목적으로 현재프레임의 애니메이션 본 행렬을 저장
		for (UINT i = 0; i < MAX_BONES; ++i)
		{
			LastFrameAnimMatrices[i]= FinalBoneMatrices[i];
		}
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

void UAnimInstance::Montage_Play(std::shared_ptr<UAnimMontage> MontageToPlay, float InTimeToStartMontageAt, const Delegate<>& OnMontageEnd, const Delegate<>& OnMontageBlendingInStart, const Delegate<>& OnMontageBlendOutStart)
{
	if (!MontageToPlay || MontageToPlay->GetPlayLength() <= 0.0f)
	{
		return;
	}

	// 이미 해당 몽타쥬가 재생중인지 확인
	for (const auto& MontageInstance : MontageInstances)
	{
		if (MontageInstance->Montage == MontageToPlay)
		{
			MontageInstance->SetPosition(std::clamp(InTimeToStartMontageAt, 0.0f, MontageToPlay->GetPlayLength()));
			return;
		}
	}

	auto LastMontageSameSlot = std::find_if(MontageInstances.begin(), MontageInstances.end(), [MontageToPlay](const std::shared_ptr<FAnimMontageInstance>& MontageInstance)
	{
		return MontageInstance->Montage->SlotName == MontageToPlay->SlotName;
	});
	if (LastMontageSameSlot != MontageInstances.end())
	{
		MY_LOG("LOG",EDebugLogLevel::DLL_Display, "Erase Last "+(*LastMontageSameSlot)->Montage->SlotName + " Slot");
		MontageInstances.erase(LastMontageSameSlot);	
	}
	

	auto NewInstance     = std::make_shared<FAnimMontageInstance>(this);
	NewInstance->Montage = MontageToPlay;
	for (const std::shared_ptr<UAnimSequence>& AS : NewInstance->Montage->AnimTrack.AnimSegments)
	{
		AS->LastUpdateTime = 0.0f;
	}

	NewInstance->SetPosition(std::clamp(InTimeToStartMontageAt, 0.0f, MontageToPlay->GetPlayLength()));
	NewInstance->CurrentPlayTime = 0.0f;
	NewInstance->OnMontageEnded = OnMontageEnd;
	NewInstance->OnMontageBlendInStart = OnMontageBlendingInStart;
	NewInstance->OnMontageBlendedOutStart = OnMontageBlendOutStart;
	MontageInstances.emplace_back(NewInstance);
}

bool UAnimInstance::IsAllResourceOK()
{
	bool bIsGameStart = GEngine->bGameStart;
	return GetSkeletalMeshComponent() && bIsGameStart;
}

void UAnimInstance::LayeredBlendPerBone(const std::vector<XMMATRIX>& BasePose, const std::vector<XMMATRIX>& BlendPose, const std::string& TargetBoneName, float BlendWeights, std::vector<XMMATRIX>& OutMatrices)
{
	std::map<std::string,std::vector<FPrecomputedBoneData>>& BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
	if (BoneHierarchyMap.contains(GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()))
	{
		std::vector<FPrecomputedBoneData>& BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];

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
		for (int i = 0; i < BoneHierarchy.size(); ++i)
		{
			bool bHasTargetParentBone    = false;
			int  CurrentCheckParentIndex = BoneHierarchy[i].ParentIndex;
			while (true)
			{
				if (CurrentCheckParentIndex >= 0)
				{
					// 부모 중 타겟 본이 존재
					if (BoneHierarchy[CurrentCheckParentIndex].BoneName == TargetBoneName)
					{
						bHasTargetParentBone = true;
						break;
					}
					CurrentCheckParentIndex = BoneHierarchy[CurrentCheckParentIndex].ParentIndex;
					continue;
				}
				break;
			}
			int CurrentBoneIndex = BoneHierarchy[i].BoneInfo.id;
			// 본의 계층을 돌면서 해당 본의 부모 중 TargetBoneName의 본이 있을 경우 블렌딩
			if (0 <= CurrentBoneIndex && CurrentBoneIndex < MAX_BONES)
			{
				if (bHasTargetParentBone)
				{
					OutMatrices[CurrentBoneIndex] = MyMatrixLerpForAnimation(BasePose[CurrentBoneIndex], BlendPose[CurrentBoneIndex] * ToBaseAnimTargetBoneMatrix, BlendWeights);
				}
				else
				{
					OutMatrices[CurrentBoneIndex] = BasePose[CurrentBoneIndex];
				}
			}
		}
	}
}

void UAnimInstance::PlayMontage(const std::string& SlotName, std::vector<XMMATRIX>& OriginMatrices, std::vector<FAnimNotifyEvent>& OriginNotifies)
{
	std::vector<XMMATRIX> MontageMatrices(MAX_BONES, XMMatrixIdentity());
	for (const auto& MontageInstance : MontageInstances)
	{
		if (MontageInstance->Montage->SlotName == SlotName)
		{
			OwnerCharacter->SetCurPlayingAnimMontage(MontageInstance->Montage->GetName());

			MontageMatrices = MontageInstance->MontageBones;
			// 몽타쥬가 RootMotion 애니메이션을 재생중인지를 변수로 캐싱
			bPlayRootMotion = MontageInstance->bPlayRootMotion;
			// 해당 섹션의 애니메이션을 재생중일 경우 기존의 노티파이는 모두 제거하고 해당 몽타쥬의 노티파이만 진행
			OriginNotifies          = MontageInstance->Notifies;
			float BlendInBlendTime  = MontageInstance->Montage->BlendIn.GetBlendTime();
			float BlendOutBlendTime = MontageInstance->Montage->BlendOut.GetBlendTime();

			float StartTimeFrame, EndTimeFrame;
			MontageInstance->Montage->GetSectionStartAndEndTime(static_cast<UINT>(MontageInstance->GetPosition()), StartTimeFrame, EndTimeFrame);
			float StartTime = StartTimeFrame / 30; // 30FPS
			float EndTime   = EndTimeFrame / 30;
			bBlendOut =false;
			
			if (MontageInstance->CurrentPlayTime < BlendInBlendTime)
			{
				const FRichCurve& BlendInCurve   = MontageInstance->Montage->BlendIn.GetCurve()->GetRichCurve();
				float             NormalizedTime = MontageInstance->CurrentPlayTime / BlendInBlendTime; // 0 ~ 1 의 값
				float             CurveValue     = BlendInCurve.Eval(NormalizedTime);

				for (int i = 0; i < MAX_BONES; ++i)
				{
				 	OriginMatrices[i] = MyMatrixLerpForAnimation(OriginMatrices[i],MontageMatrices[i], CurveValue);
				}
			}
			else if (EndTime - BlendOutBlendTime <= MontageInstance->CurrentPlayTime /* && MontageInstance->CurrentPlayTime <= EndTime*/)
			{
				const FRichCurve& BlendOutCurve  = MontageInstance->Montage->BlendOut.GetCurve()->GetRichCurve();
				float             NormalizedTime = (EndTime - MontageInstance->CurrentPlayTime) / BlendOutBlendTime; // 1~0의 값
				float             CurveValue     = BlendOutCurve.Eval(NormalizedTime);\
				for (int i = 0; i < MAX_BONES; ++i)
				{
					OriginMatrices[i] = MyMatrixLerpForAnimation(OriginMatrices[i],MontageMatrices[i], CurveValue);
				}	
				bBlendOut = true;
				
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
