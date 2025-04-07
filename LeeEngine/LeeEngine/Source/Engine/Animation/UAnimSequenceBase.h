// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "AnimTypes.h"
#include "UAnimationAsset.h"

class UAnimSequenceBase : public UAnimationAsset
{
	MY_GENERATED_BODY(UAnimSequenceBase)
public:
	// 더 이른 시간을 기준으로 notify 정렬
	void SortNotifies();

	// StartTime(exclusive) 와 StartTime+DeltaTime(inclusive) 사이의 Notify를 반환
	void GetAnimNotifies(const float& StartTime, const float& DeltaTime, std::vector<FAnimNotifyEvent>& OutActiveNotifies);

protected:
private:
public:
	std::vector<FAnimNotifyEvent> Notifies;
protected:
private:
};
