// 04.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/Animation/UAnimCompositeBase.h"
#include "Engine/Animation/UAnimSequence.h"

struct FCompositeSection
{
	std::string SectionName;
	std::string NextSectionName;
	float StartTime;

};
struct FAnimTrack
{
	std::vector<std::shared_ptr<UAnimSequence>> AnimSegments;

	float GetLength() const;
};

class UAnimMontage : public UAnimCompositeBase
{
	MY_GENERATED_BODY(UAnimMontage)

public:
	// 언리얼엔진은 FSlotAnimationTrack으로 여러 슬롯으로 관리할 수 있도록 하지만,
	// LeeEngine에서는 하나의 몽타쥬당 하나의 섹션만 관리하도록 진행할 예정
	std::string SlotName;
	FAnimTrack AnimTrack;


	std::vector<FCompositeSection> CompositeSections;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

protected:
private:
public:
protected:
private:
};