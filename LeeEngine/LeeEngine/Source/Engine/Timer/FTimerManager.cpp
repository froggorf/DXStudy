#include "CoreMinimal.h"
#include "FTimerManager.h"

#include "Engine/RenderCore/EditorScene.h"


FTimerHandle::FTimerHandle()
{
	static std::atomic<UINT> TimerCount = 0;
	TimerID = TimerCount++;
}

void FTimerManager::Tick(float DeltaSeconds)
{
	if (Timers.empty())
	{
		return;
	}

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

		if (!TimerData.bMustKill)
		{
			TimerData.Delegate.Broadcast();
		}

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
		RemoveTimer(RemoveHandle);
	}
}

void FTimerManager::SetTimer(const FTimerHandle& TimerHandle, const Delegate<>& TimerDelegate, float DelayTime, bool bRepeat, float RepeatTime)
{
	if (Timers.contains(TimerHandle))
	{
		return;
	}

	FTimerData TimerData;
	TimerData.Delegate = TimerDelegate;
	TimerData.CurTime = DelayTime;
	TimerData.bRepeat = bRepeat;
	TimerData.RepeatTime = RepeatTime;
	Timers.emplace(TimerHandle, TimerData);
}

void FTimerManager::ClearTimer(const FTimerHandle& ClearTimerHandle)
{
	// Timer의 repeat 를 false로 만들고
	// ::Tick 에서 제거되도록 유도
	auto ClearIter = Timers.find(ClearTimerHandle);
	if (ClearIter != Timers.end())
	{
		FTimerData& Data = ClearIter->second;
		Data.bRepeat = false;
		Data.bMustKill = true;
	}
}

void FTimerManager::RemoveTimer(const FTimerHandle& RemoveTimerHandle)
{
	auto DeleteIter = Timers.find(RemoveTimerHandle);
	if (DeleteIter != Timers.end())
	{
		Timers.erase(DeleteIter);
	}
}

