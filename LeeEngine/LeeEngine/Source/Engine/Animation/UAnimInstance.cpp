#include "CoreMinimal.h"
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "UAnimInstance.h"
#include "Engine/Class/Framework/ACharacter.h"
#include "Engine/World/UWorld.h"

UAnimInstance::UAnimInstance()
{
	DeltaTime = 0.0f;
	BoneTransforms = std::vector<FBoneLocalTransform>(MAX_BONES);
}

void UAnimInstance::BeginPlay()
{
	OwnerCharacter = dynamic_cast<ACharacter*>(GetSkeletalMeshComponent()->GetOwner());

	UObject::BeginPlay();

	NativeInitializeAnimation();

	SetAnimNotify_BeginPlay();
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
				 MontageInstances[i]->Play();
			}
			else
			{
				MontageInstances.erase(MontageInstances.begin() + i);
				--i;
			}
			
		}
	}
}

void UAnimInstance::CalculateFinalBoneMatrices(std::vector<XMMATRIX>& FinalBoneMatrices)
{
	CalculateBoneMatrices(BoneTransforms, FinalBoneMatrices);
}

void UAnimInstance::CalculateBoneMatrices(const std::vector<FBoneLocalTransform>& BoneTransforms, std::vector<XMMATRIX>& BoneMatrices)
{
	const std::string& MeshName = GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName();

	std::map<std::string,std::vector<FPrecomputedBoneData>>& BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
	std::vector<FPrecomputedBoneData>& BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];
	std::vector<XMMATRIX> GlobalTransforms(MAX_BONES, XMMatrixIdentity());
	for (int HierarchyIndex = 0; HierarchyIndex < BoneHierarchy.size(); ++HierarchyIndex)
	{
		const FPrecomputedBoneData& BoneData = BoneHierarchy[HierarchyIndex];

		XMMATRIX LocalMatrix = XMMatrixTransformation(
			XMVectorZero(), 
			XMQuaternionIdentity(),
			BoneTransforms[HierarchyIndex].Scale,
			XMVectorZero(),
			BoneTransforms[HierarchyIndex].Rotation,
			BoneTransforms[HierarchyIndex].Translation
		);

		if (BoneData.ParentIndex >= 0)
			GlobalTransforms[HierarchyIndex] = XMMatrixMultiply(LocalMatrix, GlobalTransforms[BoneData.ParentIndex]);
		else
			GlobalTransforms[HierarchyIndex] = LocalMatrix;

		if (BoneData.BoneInfo.id >= 0 && BoneData.BoneInfo.id < MAX_BONES)
			BoneMatrices[BoneData.BoneInfo.id] = XMMatrixMultiply(BoneData.BoneInfo.offset, GlobalTransforms[HierarchyIndex]);
	}
}

void UAnimInstance::DecomposeBoneMatricesToBoneLocalTransForms(const std::vector<XMMATRIX>& BoneMatrices, std::vector<FBoneLocalTransform>& BoneTransforms)
{
	std::map<std::string,std::vector<FPrecomputedBoneData>>& BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
	auto BoneHierarchyIter = BoneHierarchyMap.find(GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName());
	if (BoneHierarchyIter == BoneHierarchyMap.end())
	{
		return;
	}

	std::vector<FPrecomputedBoneData>& BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];

	// 글로벌 본 행렬을 먼저 계산했다고 가정
	for (size_t i = 0; i < BoneHierarchy.size(); ++i)
	{
		XMMATRIX ParentGlobalMatrix = XMMatrixIdentity();
		int ParentIndex = BoneHierarchy[i].ParentIndex;
		if (ParentIndex >= 0)
			ParentGlobalMatrix = BoneMatrices[ParentIndex];

		// 부모 기준으로 로컬 행렬 추출
		XMMATRIX LocalMatrix = XMMatrixMultiply(XMMatrixInverse(nullptr, ParentGlobalMatrix), BoneMatrices[i]);

		// SRT 분해 (Scale, Rotation, Translation)
		FBoneLocalTransform& Out = BoneTransforms[i];
		XMMatrixDecompose(&Out.Scale, &Out.Rotation, &Out.Translation, LocalMatrix);
	}
}

void UAnimInstance::Tick(float DeltaSeconds)
{
	DeltaTime                 = DeltaSeconds;
	static float DefaultSpeed = 30;
	CurrentTime               = CurrentTime + DeltaSeconds * DefaultSpeed;

	static float TicksPerSecondTime = 1.0f / AnimTickFPS * DefaultSpeed;
	if (LatestUpdateTime + TicksPerSecondTime < CurrentTime)
	{
		if (!bIsAnimationLoaded)
		{
			const std::string& MeshName = GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName();
			if (GEngine->GetTimeSeconds() - 1.0f >= UAnimSequence::GetLoadedTime(MeshName))
			{
				bIsAnimationLoaded = true;
			}
			else
			{
				return;
			}
		}

		bPlayRootMotion = false;
		LatestUpdateTime = CurrentTime;

		NativeUpdateAnimation(DeltaSeconds);
		UpdateAnimation(DeltaSeconds);

		// 노티파이 실행
		for (int i = 0; i < FinalNotifies.size(); ++i)
		{
			const std::string& NotifyName = FinalNotifies[i].Notify->Notify();

			const auto& Notify = NotifyEvent.find(NotifyName);
			if (Notify != NotifyEvent.end())
			{
				Notify->second.Broadcast();
			}
		}
		FinalNotifies.clear();

		// FinalBoneMatrices 계산
		std::vector<XMMATRIX> FinalBoneMatrices(MAX_BONES,XMMatrixIdentity());
		CalculateFinalBoneMatrices(FinalBoneMatrices);

		/// 루트 모션 처리
		if (APlayerController* PC = GEngine->GetCurrentWorld()->GetPlayerController())
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
			LastFrameAnimMatrices[i] = FinalBoneMatrices[i];
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

std::vector<FBoneLocalTransform> UAnimInstance::GetInitialLocalBoneTransforms() const
{
	//if (!bIsAnimationLoaded)
	//{
	//	return std::vector<FBoneLocalTransform>(MAX_BONES);
	//}

	std::vector<FBoneLocalTransform> BoneTransforms(MAX_BONES);
	bool Dummy;
	GetSkeletalMeshComponent()->GetSkeletalMesh()->GetDefaultAnimSequence()->GetBoneTransform(0, BoneTransforms, &Dummy);
	return BoneTransforms;
}

bool UAnimInstance::IsAllResourceOK()
{
	bool bIsGameStart = GEngine->bGameStart;
	return GetSkeletalMeshComponent() && bIsGameStart;
}

void UAnimInstance::LayeredBlendPerBone(const std::vector<FBoneLocalTransform>& BasePoseBoneTransforms, const std::vector<FBoneLocalTransform>& BlendPoseBoneTransForms, const std::string& TargetBoneName, float BlendWeights, std::vector<FBoneLocalTransform>& OutBoneTransforms)
{
	if (!bIsAnimationLoaded)
	{
		return;
	}

	std::map<std::string,std::vector<FPrecomputedBoneData>>& BoneHierarchyMap = UAnimSequence::GetSkeletonBoneHierarchyMap();
	std::vector<FPrecomputedBoneData>& BoneHierarchy = BoneHierarchyMap[GetSkeletalMeshComponent()->GetSkeletalMesh()->GetName()];

	for (int i = 0; i < BoneHierarchy.size(); ++i)
	{
		bool bHasTargetParentBone = false;
		int CurrentCheckParentIndex = BoneHierarchy[i].ParentIndex;
		while (true)
		{
			if (CurrentCheckParentIndex >= 0)
			{
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
		if (0 <= CurrentBoneIndex && CurrentBoneIndex < MAX_BONES)
		{
			if (bHasTargetParentBone)
			{
				OutBoneTransforms[CurrentBoneIndex] = Blend2BoneTransform(BasePoseBoneTransforms[CurrentBoneIndex], BlendPoseBoneTransForms[CurrentBoneIndex], BlendWeights);
			}
			else
			{
				OutBoneTransforms[CurrentBoneIndex] = BasePoseBoneTransforms[CurrentBoneIndex];
			}
		}
	}
}
void UAnimInstance::PlayMontage(const std::string& SlotName, std::vector<FBoneLocalTransform>& OriginBoneTransforms, std::vector<FAnimNotifyEvent>& OriginNotifies)
{
	std::vector<FBoneLocalTransform> MontageBoneTransforms(MAX_BONES);
	for (const auto& MontageInstance : MontageInstances)
	{
		if (MontageInstance->Montage->SlotName == SlotName)
		{
			OwnerCharacter->SetCurPlayingAnimMontage(MontageInstance->Montage->GetName());

			MontageBoneTransforms = MontageInstance->MontageBones;
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

			// 블렌드 In
			if (StartTime > 0.0f && MontageInstance->CurrentPlayTime < BlendInBlendTime)
			{
				const FRichCurve& BlendInCurve   = MontageInstance->Montage->BlendIn.GetCurve()->GetRichCurve();
				float             NormalizedTime = MontageInstance->CurrentPlayTime / BlendInBlendTime; // 0 ~ 1 의 값
				float             CurveValue     = BlendInCurve.Eval(NormalizedTime);

				for (int i = 0; i < MAX_BONES; ++i)
				{
					OriginBoneTransforms[i] = Blend2BoneTransform(OriginBoneTransforms[i], MontageBoneTransforms[i], CurveValue);
				}
			}
			// 블렌드 Out
			else if (BlendOutBlendTime > 0.0f && EndTime - BlendOutBlendTime <= MontageInstance->CurrentPlayTime /* && MontageInstance->CurrentPlayTime <= EndTime*/)
			{
				const FRichCurve& BlendOutCurve  = MontageInstance->Montage->BlendOut.GetCurve()->GetRichCurve();
				float             NormalizedTime = (EndTime - MontageInstance->CurrentPlayTime) / BlendOutBlendTime; // 1~0의 값
				float             CurveValue     = BlendOutCurve.Eval(NormalizedTime);\
				for (int i = 0; i < MAX_BONES; ++i)
				{
					OriginBoneTransforms[i] = Blend2BoneTransform(OriginBoneTransforms[i], MontageBoneTransforms[i], CurveValue);
				}
				bBlendOut = true;

				if (!MontageInstance->bIsBlendOutBroadcast)
				{
					MontageInstance->bIsBlendOutBroadcast = true;
					MontageInstance->OnMontageBlendedOutStart.Broadcast();
				}
			}
			else
			{
				OriginBoneTransforms = MontageBoneTransforms;
			}
		}
	}
}

/*


 */
