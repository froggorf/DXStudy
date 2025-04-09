#include "CoreMinimal.h"
#include "UAnimMontage.h"

float FAnimTrack::GetLength() const
{
	float TotalLength = 0.0f;
	for(const auto& AnimSegment : AnimSegments)
	{
		TotalLength += AnimSegment->GetLength();
	}
	return TotalLength;
}

void UAnimMontage::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimCompositeBase::LoadDataFromFileData(AssetData);

	if(AssetData.contains("AnimTrack"))
	{
		auto AnimSequencesData = AssetData["AnimTrack"];
		for(const auto& SequenceData : AnimSequencesData)
		{
			AnimTrack.AnimSegments.emplace_back(UAnimSequence::GetAnimationAsset(SequenceData["AnimName"]));
		}
	}

	if(AssetData.contains("Sections"))
	{
		auto SectionsData = AssetData["Sections"];
		for(const auto& SectionData : SectionsData)
		{
			FCompositeSection NewSection;
			NewSection.SectionName = SectionData["SectionName"];
			NewSection.StartTime = SectionData["StartTime"];
			if(SectionData.contains("NextSectionName"))
			{
				NewSection.NextSectionName = SectionData["NextSectionName"];	
			}
			CompositeSections.emplace_back(NewSection);
		}
		
	}


}
