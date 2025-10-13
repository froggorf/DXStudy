// 09.26
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"

#include "Engine/Misc/Delegate.h"


struct FTimerHandle
{
	FTimerHandle();
	UINT TimerID;

	bool operator<(const FTimerHandle& Other) const
	{
		return TimerID < Other.TimerID;
	}
};

struct FTimerData
{
	FTimerData() = default;
	virtual ~FTimerData() = default;

	float RepeatTime = 1.0f;
	bool bRepeat = false;
	bool bMustKill = false;

	float CurTime = 1.0f;

	Delegate<> Delegate;
};


class FTimerManager
{
public:
	void Tick(float DeltaSeconds);

	void SetTimer(const FTimerHandle& TimerHandle, const Delegate<>& TimerDelegate, float DelayTime, bool bRepeat = false, float RepeatTime = 1.0f);

	void ClearTimer(const FTimerHandle& ClearTimerHandle);
protected:
	void RemoveTimer(const FTimerHandle& RemoveTimerHandle);
private:
	std::map<FTimerHandle, FTimerData> Timers;
};