#include "CoreMinimal.h"
#include "UAnimMontage.h"

#include "Engine/Animation/UAnimInstance.h"
#include "Engine/AssetManager/AssetManager.h"

float FAnimTrack::GetLength() const
{
	float TotalLength = 0.0f;
	for (const auto& AnimSegment : AnimSegments)
	{
		TotalLength += AnimSegment->GetLength();
	}
	return TotalLength;
}

std::shared_ptr<UAnimMontage> UAnimMontage::GetAnimationAsset(const std::string& AnimationName)
{
	if (std::shared_ptr<UAnimationAsset> FindAsset = UAnimationAsset::GetAnimationAsset(AnimationName))
	{
		return std::dynamic_pointer_cast<UAnimMontage>(FindAsset);
	}
	return nullptr;
}

void UAnimMontage::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimCompositeBase::LoadDataFromFileData(AssetData);

	SlotName = AssetData["SlotName"];
	if (AssetData.contains("AnimTrack"))
	{
		const auto& AnimSequencesData = AssetData["AnimTrack"];
		int AnimTrackSize = static_cast<int>(AnimSequencesData.size());
		AnimTrack.AnimSegments.resize(AnimTrackSize);
		std::atomic<int> CurrentLoadedAnimCount = 0;
		for (size_t i = 0; i < AnimSequencesData.size(); ++i)
		{
			AssetManager::GetAsyncAssetCache(AnimSequencesData[i]["AnimName"], [this, &CurrentLoadedAnimCount, i](std::shared_ptr<UObject> AnimSequence)
				{
					if (!AnimSequence)
					{
						assert(nullptr && "잘못된 로드");
					}
					AnimTrack.AnimSegments[i] = (std::static_pointer_cast<UAnimSequence>(AnimSequence));
					++CurrentLoadedAnimCount;
				});
		}

		while (CurrentLoadedAnimCount != AnimTrackSize)
		{
			std::this_thread::yield();
		}
	}

	if (AssetData.contains("Sections"))
	{
		auto SectionsData = AssetData["Sections"];
		for (const auto& SectionData : SectionsData)
		{
			FCompositeSection NewSection;
			NewSection.SectionName = SectionData["SectionName"];
			NewSection.StartTime   = SectionData["StartTime"];
			if (SectionData.contains("NextSectionName"))
			{
				NewSection.NextSectionName = SectionData["NextSectionName"];
			}
			CompositeSections.emplace_back(NewSection);
		}
	}

	// 반드시 있어야 하는 데이터
	{
		// BlendIn
		auto                         BlendInData      = AssetData["BlendIn"];
		std::string                  BlendInCurveName = BlendInData["CurveName"];
		std::shared_ptr<UCurveFloat> CustomCurveFloat = std::dynamic_pointer_cast<UCurveFloat>(UCurveBase::GetCurveAssetCache(BlendInCurveName));
		assert(CustomCurveFloat);
		BlendIn.SetCurveFloat(CustomCurveFloat);
		float BlendInBlendTime = BlendInData["BlendTime"];
		BlendIn.SetBlendTime(BlendInBlendTime);

		// BlendOut
		auto                         BlendOutData             = AssetData["BlendOut"];
		std::string                  BlendOutCurveName        = BlendOutData["CurveName"];
		std::shared_ptr<UCurveFloat> BlendOutCustomCurveFloat = std::dynamic_pointer_cast<UCurveFloat>(UCurveBase::GetCurveAssetCache(BlendOutCurveName));
		assert(BlendOutCustomCurveFloat);
		BlendOut.SetCurveFloat(BlendOutCustomCurveFloat);
		float BlendOutBlendTime = BlendOutData["BlendTime"];
		BlendOut.SetBlendTime(BlendOutBlendTime);
	}


}

float UAnimMontage::GetStartTimeFromSectionName(const std::string& SectionName)
{
	int SectionID = GetSectionIndex(SectionName);
	if (0 <= SectionID)
	{
		return CompositeSections[SectionID].StartTime;
	}
	MY_LOG("GetStartTimeFromSectionName", EDebugLogLevel::DLL_Error, "Invalid section name")
	return 0.0f;
}

void UAnimMontage::GetSectionStartAndEndTime(UINT SectionIndex, float& OutStartTime, float& OutEndTime)
{
	OutStartTime = 0.0f;
	OutEndTime   = GetPlayLength();

	if (SectionIndex < CompositeSections.size())
	{
		OutStartTime = CompositeSections[SectionIndex].StartTime;
	}
	if (SectionIndex + 1 < CompositeSections.size())
	{
		OutEndTime = CompositeSections[SectionIndex + 1].StartTime;
	}
}

bool UAnimMontage::IsWithinPos(UINT SectionIndex1, UINT SectionIndex2, float InPosition)
{
	float StartTime = 0.0f, EndTime = GetPlayLength();
	if (SectionIndex1 < CompositeSections.size())
	{
		StartTime = CompositeSections[SectionIndex1].StartTime;
	}
	if (SectionIndex2 < CompositeSections.size())
	{
		EndTime = CompositeSections[SectionIndex2].StartTime;
	}

	return (StartTime <= InPosition && InPosition < EndTime);	
	
	
}

int UAnimMontage::GetSectionIndexFromPosition(float InPosition)
{
	for (UINT i = 0; i < CompositeSections.size(); ++i)
	{
		if (IsWithinPos(i, i + 1, InPosition))
		{
			return i;
		}
	}
	return -1;
}

int UAnimMontage::GetSectionIndex(const std::string& InSectionName) const
{
	for (UINT i = 0; i < CompositeSections.size(); ++i)
	{
		if (CompositeSections[i].SectionName == InSectionName)
		{
			return i;
		}
	}
	return -1;
}

void FAnimMontageInstance::Play()
{
	bIsPlaying = true;

	int CurrentPlayingSectionIndex = Montage->GetSectionIndexFromPosition(Position < 0.0f? -Position : Position);
	std::shared_ptr<UAnimSequence> Anim = Montage->AnimTrack.AnimSegments[CurrentPlayingSectionIndex];
	Position += 1.0f * (30.0f/UAnimInstance::AnimTickFPS) * Anim->GetRateScale();

	if (ImGui::IsKeyDown(ImGuiKey_5))
	{
		int a= 0;
	}
	// 현재 섹션 끝났을 때
	if (std::abs(Position) >= CurPlayingEndPosition)
	{
		// 다음 섹션이 있다면
		if (!NextSectionName.empty())
		{
			int NextSectionID = Montage->GetSectionIndex(NextSectionName);
			SetPosition(Montage->CompositeSections[NextSectionID].StartTime);

			CurrentPlayingSectionIndex = NextSectionID;
			Anim = Montage->AnimTrack.AnimSegments[CurrentPlayingSectionIndex];
		}
		else
		{
			OnMontageEnded.Broadcast();
			bIsPlaying = false;
			return;
		}
	}
	const std::vector<std::shared_ptr<UAnimSequence>> AnimSegments        = Montage->AnimTrack.AnimSegments;

	float CurrentAnimPosition = Position;
	if (CurrentPlayingSectionIndex != -1)
	{
		CurrentAnimPosition -= Montage->CompositeSections[CurrentPlayingSectionIndex].StartTime;	
	}
	Anim->GetBoneTransform_NoRateScale(CurrentAnimPosition, MontageBones, &bPlayRootMotion);
	
	CurrentPlayTime = Position < 0.0f? -Position/30 : Position/30;
	Notifies.clear();
	Montage->GetAnimNotifies(Position, Notifies);
}

void FAnimMontageInstance::SetPosition(float InPosition)
{
	Position      = InPosition;
	CurrentPlayTime = Position;
	int SectionID = Montage->GetSectionIndexFromPosition(Position);
	if (SectionID >= 0)
	{
		Montage->GetSectionStartAndEndTime(SectionID, CurPlayingStartPosition, CurPlayingEndPosition);
		NextSectionName = Montage->CompositeSections[SectionID].NextSectionName;
		bIsBlendOutBroadcast = false;
		bIsBlendInBroadcast = false;
	}
}

void FAnimMontageInstance::JumpToSectionName(const std::string& SectionName)
{
	const int SectionID = Montage->GetSectionIndex(SectionName);

	if (0 <= SectionID)
	{
		FCompositeSection CurSection  = Montage->CompositeSections[SectionID];
		const float       NewPosition = CurSection.StartTime;
		SetPosition(NewPosition);
	}
}
