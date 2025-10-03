// 04.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"
#include "UAnimSequence.h"
#include "UBlendSpace.h"

// ==============================================================
//
//class UAimOffsetBlendSpace : public UBlendSpace
//{
//	MY_GENERATE_BODY(UAimOffsetBlendSpace);
//	UAimOffsetBlendSpace();
//
//	static std::shared_ptr<UAimOffsetBlendSpace> GetAnimationAsset(const std::string& AnimationName);
//
//	// UBlendSpace와 다르게 AimOffset의 정중앙의 애니메이션과의 "Offset" 에 대한 본 매트릭스만 반환
//	void GetAnimationBoneTransforms(const XMFLOAT2& AnimValue, float CurrentAnimTime, std::vector<XMMATRIX>& OutMatrices, std::vector<FAnimNotifyEvent>& OutActiveNotifies) override;
//
//	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
//
//private:
//	std::vector<XMMATRIX> DefaultAnimMatrices;
//};
