// 04.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "AnimNotifies/AnimNotify.h"

struct FAnimNotifyEvent
{
	FAnimNotifyEvent();

public:
	float                        TriggerTimeOffset;
	float                        EndTriggerTimeOffset;
	std::shared_ptr<UAnimNotify> Notify;
	//std::shared_ptr<UAnimNotifyState> NotifyState;

	bool operator<(const FAnimNotifyEvent& Other) const
	{
		return TriggerTimeOffset < Other.TriggerTimeOffset;
	}

	float GetTriggerTime() const
	{
		return TriggerTimeOffset;
	}

	float GetEndTriggerTime() const
	{
		return EndTriggerTimeOffset;
	}
};
