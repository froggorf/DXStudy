// 04.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"
#include "UAnimationAsset.h"
#include "UAnimSequence.h"
#include "UBlendSpace.h"

// ==============================================================

class UAimOffsetBlendSpace : public UBlendSpace
{
	MY_GENERATED_BODY(UAimOffsetBlendSpace);
public:
	UAimOffsetBlendSpace();

	inline static const std::shared_ptr<UAimOffsetBlendSpace>& GetAnimationAsset(const std::string& AnimationName)
	{
		if(std::shared_ptr<UAnimationAsset> FindAsset = UAnimationAsset::GetAnimationAsset(AnimationName))
		{
			return std::dynamic_pointer_cast<UAimOffsetBlendSpace>(FindAsset);	
		}
		return nullptr;
	}
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
};