// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "AnimTypes.h"
#include "UAnimationAsset.h"

class UAnimSequenceBase : public UAnimationAsset
{
	MY_GENERATE_BODY(UAnimSequenceBase)
	// 더 이른 시간을 기준으로 notify 정렬
	void SortNotifies();

	// LastUpdateTime 과 CurrentTime 사이의 Notify를 반환
	void GetAnimNotifies(const float& CurrentTime, std::vector<FAnimNotifyEvent>& OutActiveNotifies);

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	std::vector<FAnimNotifyEvent> Notifies;

	float LastUpdateTime        = 1000.0f;
	float LastUpdateTimeSeconds = 0.0f;
};
