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

	float CurTime = 1.0f;

	Delegate<> Delegate;
};


class FTimerManager
{
public:
	void Tick(float DeltaSeconds)
	{
		std::vector<FTimerHandle> RemoveHandles;
		for (auto& Timer : Timers)
		{
			const FTimerHandle& TimerHandle = Timer.first;
			FTimerData& TimerData = Timer.second;
			TimerData.CurTime -= DeltaSeconds;
			if (TimerData.CurTime > 0.0f)
			{
				continue;
			}
			
			TimerData.Delegate.Broadcast();
			if (TimerData.bRepeat)
			{
				TimerData.CurTime += TimerData.RepeatTime;
			}
			else
			{
				RemoveHandles.emplace_back(TimerHandle);
			}
		}

		for (const FTimerHandle& RemoveHandle : RemoveHandles)
		{
			ClearTimer(RemoveHandle);
		}
	}

	void SetTimer(const FTimerHandle& TimerHandle, const Delegate<>& TimerDelegate, float DelayTime, bool bRepeat = false, float RepeatTime = 1.0f)
	{
		FTimerData TimerData;
		TimerData.Delegate = TimerDelegate;
		TimerData.CurTime = DelayTime;
		TimerData.bRepeat = bRepeat;
		TimerData.RepeatTime = RepeatTime;
		Timers.emplace(TimerHandle, TimerData);
	}

	void ClearTimer(const FTimerHandle& DeleteTimerHandle)
	{
		auto DeleteIter = Timers.find(DeleteTimerHandle);
		if (DeleteIter != Timers.end())
		{
			Timers.erase(DeleteIter);
		}
	}
protected:
private:
	std::map<FTimerHandle, FTimerData> Timers;
};