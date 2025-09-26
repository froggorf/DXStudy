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

struct FTimerDataBase
{
	virtual ~FTimerDataBase() = default;

	float RepeatTime = 1.0f;
	bool bRepeat = false;

	float CurTime = 1.0f;

	virtual void ExecuteDelegate(){}
};

template<typename... Arg>
struct FTimerData : FTimerDataBase
{
	FTimerData(const Delegate<Arg>& InDelegate) : Delegate(InDelegate) {}
	~FTimerData() override = default;

	void ExecuteDelegate() override
	{
		Delegate.Broadcast();
	}

	Delegate<Arg> Delegate;
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
			const std::shared_ptr<FTimerDataBase>& TimerData = Timer.second;
			TimerData->CurTime -= DeltaSeconds;
			if (TimerData->CurTime > 0.0f)
			{
				continue;
			}
			
			TimerData->ExecuteDelegate();
			if (TimerData->bRepeat)
			{
				TimerData->CurTime += TimerData->RepeatTime;
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

	template<typename... Args>
	void SetTimer(const FTimerHandle& TimerHandle, Delegate<Args> Delegate, float DelayTime, bool bRepeat = false, float RepeatTime = 1.0f)
	{
		std::shared_ptr<FTimerDataBase> TimerData = std::make_shared<FTimerData<Args>>(Delegate);
		TimerData->CurTime = DelayTime;
		TimerData->bRepeat = bRepeat;
		TimerData->RepeatTime = RepeatTime;
		Timers[TimerHandle] = TimerData;
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
	std::map<FTimerHandle, std::shared_ptr<FTimerDataBase>> Timers;
};